#include "system.hpp"

// stl
#include <filesystem>
#include <fstream>
#include <future>
#include <unordered_map>

// entt
#include <entt/entity/registry.hpp>

// polyvox
#include <PolyVox/RawVolume.h>
#include <PolyVox/CubicSurfaceExtractor.h>
#include <PolyVox/VolumeResampler.h>

// putils
#include "putils/forward_to.hpp"
#include "putils/string.hpp"
#include "putils/thread_name.hpp"

// kengine core
#include "kengine/core/data/transform.hpp"
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/helpers/new_entity_processor.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine async
#include "kengine/async/helpers/start_task.hpp"
#include "kengine/async/helpers/process_results.hpp"

// kengine main_loop
#include "kengine/main_loop/functions/execute.hpp"

// kengine render
#include "kengine/render/data/model_data.hpp"
#include "kengine/render/data/asset.hpp"

// kengine render/polyvox
#include "kengine/render/polyvox/data/polyvox.hpp"
#include "kengine/render/polyvox/magica_voxel/helpers/format.hpp"

namespace kengine::render::polyvox::magica_voxel {
	static constexpr auto log_category = "render_polyvox_magica_voxel";

	static auto build_mesh(PolyVox::RawVolume<polyvox::vertex_data> && volume) noexcept {
		KENGINE_PROFILING_SCOPE;
		const auto encoded_mesh = PolyVox::extractCubicMesh(&volume, volume.getEnclosingRegion());
		const auto mesh = PolyVox::decodeMesh(encoded_mesh);
		return mesh;
	}

	struct system {
		entt::registry & r;

		struct processed {};
		kengine::new_entity_processor<processed, render::asset> processor{ r, putils_forward_to_this(load_model) };

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));
			processor.process();
		}

		using mesh_type = decltype(build_mesh(PolyVox::RawVolume<polyvox::vertex_data>{ {} }));

		struct model_and_offset {
			mesh_type mesh;
			format::chunk_content::size offset_to_apply;
		};

		struct async_loaded_data {
			model_data model_data;
			format::chunk_content::size offset_to_apply;
		};

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");

			processor.process();

			kengine::async::process_results<async_loaded_data>(r, [this](entt::entity e, async_loaded_data && loaded_data) {
				r.emplace<model_data>(e, std::move(loaded_data.model_data));
				apply_offset(e, loaded_data.offset_to_apply);
			});
		}

		void load_model(entt::entity e, const render::asset & asset) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Loading model for %s", asset.file.c_str());

			const auto & f = asset.file.c_str();
			if (std::filesystem::path(f).extension() != ".vox")
				return;

			kengine::async::start_task(
				r, e,
				async::task::string("magica_voxel: load %s", f),
				std::async(std::launch::async, [this, e, &f] {
					const putils::scoped_thread_name thread_name(putils::string<64>("Load %s", f));
					return load_model_data(e, f);
				})
			);
		}

		async_loaded_data load_model_data(entt::entity e, const char * file) noexcept {
			KENGINE_PROFILING_SCOPE;

			const putils::string<256> binary_file("%s.bin", file);

			if (!std::filesystem::exists(binary_file.c_str())) {
				kengine_log(r, verbose, log_category, "Binary file does not exist, creating it");
				const auto model_and_offset = load_vox_model(file);
				const auto model_data = generate_model_data(e, model_and_offset.mesh);
				serialize(binary_file.c_str(), model_data, model_and_offset.offset_to_apply);
			}

			async_loaded_data ret;
			unserialize(binary_file.c_str(), ret.model_data.meshes.emplace_back(), ret.offset_to_apply);
			ret.model_data.free = release(e);
			ret.model_data.init<mesh_type::VertexType>();
			return ret;
		}

		void unserialize(const char * f, model_data::mesh & mesh_data, format::chunk_content::size & size) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Unserializing from %s", f);

			std::ifstream file(f, std::ofstream::binary);
			if (!file) {
				kengine_assert_failed(r, "[magica_voxel] Failed to load '", f, "'");
				return;
			}

			const auto parse = [&](auto & val) noexcept {
				file.read((char *)&val, sizeof(val));
			};

			{
				parse(mesh_data.vertices.nb_elements);
				parse(mesh_data.vertices.element_size);
				const auto vertex_buffer_size = mesh_data.vertices.nb_elements * mesh_data.vertices.element_size;
				mesh_data.vertices.data = new char[vertex_buffer_size];
				file.read((char *)mesh_data.vertices.data, vertex_buffer_size);
			}

			{
				parse(mesh_data.indices.nb_elements);
				parse(mesh_data.indices.element_size);
				const auto index_buffer_size = mesh_data.indices.nb_elements * mesh_data.indices.element_size;
				mesh_data.indices.data = new char[index_buffer_size];
				file.read((char *)mesh_data.indices.data, index_buffer_size);
			}

			parse(mesh_data.index_type);

			parse(size);
		}

		model_data::free_func release(entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			return [this, e] {
				if (const auto model_data = r.try_get<render::model_data>(e)) {
					kengine_logf(r, verbose, log_category, "Releasing data for [%u]", e);
					delete[] (const char *)model_data->meshes[0].vertices.data;
					delete[] (const char *)model_data->meshes[0].indices.data;
				}
			};
		}

		model_and_offset load_vox_model(const char * f) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Loading vox model from %s", f);

			std::ifstream stream(f, std::ios::binary);
			if (!stream) {
				kengine_assert_failed(r, "[magica_voxel] Failed to load '", f, "'");
				return model_and_offset{};
			}

			check_header(stream);

			format::chunk_header main;
			read_from_stream(main, stream);
			if (!id_matches(main.id, "MAIN")) {
				kengine_assert_failed(r, "[magica_voxel] Expected 'MAIN' chunk header in '", f, "'");
				return model_and_offset{};
			}

			format::chunk_header first;
			read_from_stream(first, stream);
			if (!id_matches(first.id, "SIZE")) {
				kengine_assert_failed(r, "[magica_voxel] Expected 'SIZE' chunk header in '", f, "'");
				return model_and_offset{};
			}
			kengine_assert(r, first.children_bytes == 0);
			kengine_assert(r, first.content_bytes == sizeof(format::chunk_content::size));

			format::chunk_content::size size;
			read_from_stream(size, stream);

			PolyVox::RawVolume<polyvox::vertex_data> volume(PolyVox::Region{
				{ 0, 0, 0 },
				{ size.x, size.z, size.y },
			});

			format::chunk_header voxels_header;
			read_from_stream(voxels_header, stream);
			if (!id_matches(voxels_header.id, "XYZI")) {
				kengine_assert_failed(r, "[magica_voxel] Expected 'XYZI' chunk header in '", f, "'");
				return model_and_offset{};
			}

			format::chunk_content::rgba rgba;

			format::chunk_content::xyzi xyzi;
			read_from_stream(xyzi, stream);
			kengine_assert(r, voxels_header.content_bytes == sizeof(xyzi) + xyzi.num_voxels * sizeof(int));

			for (int i = 0; i < xyzi.num_voxels; ++i) {
				format::chunk_content::xyzi::voxel voxel;
				read_from_stream(voxel, stream);

				const auto & color = rgba.palette[voxel.color_index];
				const polyvox::vertex_data voxel_value{ { (float)color.r / 255.f, (float)color.g / 255.f, (float)color.b / 255.f } };
				volume.setVoxel(voxel.x, voxel.z, voxel.y, voxel_value);
			}

			return { build_mesh(std::move(volume)), size };
		}

		bool id_matches(const char * s1, const char * s2) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Checking ID %s", s2);
			return strncmp(s1, s2, 4) == 0;
		}

		void check_header(std::istream & s) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Checking header");

			format::file_header header;
			read_from_stream(header, s);
			kengine_assert(r, id_matches(header.id, "VOX "));
			kengine_assert(r, header.version_number == 150);
		}

		model_data generate_model_data(entt::entity e, const mesh_type & mesh) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Generating model data for [%u]", e);

			model_data model_data;
			model_data.free = release(e);
			model_data.init<mesh_type::VertexType>();

			auto & mesh_data = model_data.meshes.emplace_back();
			mesh_data.vertices = { mesh.getNoOfVertices(), sizeof(putils_typeof(mesh)::VertexType), mesh.getRawVertexData() };
			mesh_data.indices = { mesh.getNoOfIndices(), sizeof(putils_typeof(mesh)::IndexType), mesh.getRawIndexData() };
			mesh_data.index_type = putils::meta::type<putils_typeof(mesh)::IndexType>::index;

			return model_data;
		}

		void apply_offset(entt::entity e, const format::chunk_content::size & size) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (r.all_of<core::transform>(e)) {
				kengine_logf(r, verbose, log_category, "[%u] already has a core::transform. Mesh offset will not be applied", e);
				return;
			}
			kengine_log(r, log, log_category, "Applying mesh offset");

			auto & box = r.emplace<core::transform>(e).bounding_box;
			box.position.x -= size.x / 2.f * box.size.x;
			box.position.z -= size.y / 2.f * box.size.z;
		}

		void serialize(const char * f, const model_data & model_data, const format::chunk_content::size & size) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Serializing to %s", f);

			std::ofstream file(f, std::ofstream::binary | std::ofstream::trunc);
			if (!file) {
				kengine_assert_failed(r, "[magica_voxel] Failed to serialize to '", f, "'");
				return;
			}

			const auto write = [&](const auto & val) noexcept {
				file.write((const char *)&val, sizeof(val));
			};

			const auto & mesh_data = model_data.meshes.back();

			{
				write(mesh_data.vertices.nb_elements);
				write(mesh_data.vertices.element_size);
				const auto vertex_buffer_size = mesh_data.vertices.nb_elements * mesh_data.vertices.element_size;
				file.write((const char *)mesh_data.vertices.data, vertex_buffer_size);
			}

			{
				write(mesh_data.indices.nb_elements);
				write(mesh_data.indices.element_size);
				const auto index_buffer_size = mesh_data.indices.nb_elements * mesh_data.indices.element_size;
				file.write((const char *)mesh_data.indices.data, index_buffer_size);
			}

			write(mesh_data.index_type);

			write(size);
		}

		template<typename T>
		void read_from_stream(T & header, std::istream & s) noexcept {
			KENGINE_PROFILING_SCOPE;
			s.read((char *)&header, sizeof(header));
			kengine_assert(r, s.gcount() == sizeof(header));
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		system,
		system::processed
	)
}
