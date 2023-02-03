#include "magica_voxel.hpp"

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

// kengine data
#include "kengine/data/async_task.hpp"
#include "kengine/data/model_data.hpp"
#include "kengine/data/model.hpp"
#include "kengine/data/transform.hpp"
#include "kengine/data/polyvox.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/assert_helper.hpp"
#include "kengine/helpers/async_helper.hpp"
#include "kengine/helpers/instance_helper.hpp"
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

// local
#include "magica_voxel_format.hpp"

namespace kengine::systems {
	static auto build_mesh(PolyVox::RawVolume<data::polyvox::vertex_data> && volume) noexcept {
		KENGINE_PROFILING_SCOPE;
		const auto encoded_mesh = PolyVox::extractCubicMesh(&volume, volume.getEnclosingRegion());
		const auto mesh = PolyVox::decodeMesh(encoded_mesh);
		return mesh;
	}

	struct magica_voxel {
		entt::registry & r;
		entt::scoped_connection connection;

		magica_voxel(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "Init", "systems/magica_voxel");

			connection = r.on_construct<data::model>().connect<&magica_voxel::load_model>(this);
			e.emplace<functions::execute>(putils_forward_to_this(execute));
		}

		using mesh_type = decltype(build_mesh(PolyVox::RawVolume<data::polyvox::vertex_data>{ {} }));

		struct model_and_offset {
			mesh_type mesh;
			magica_voxel_format::chunk_content::size offset_to_apply;
		};

		struct async_loaded_data {
			data::model_data model_data;
			magica_voxel_format::chunk_content::size offset_to_apply;
		};

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, "execute", "systems/magica_voxel");

			kengine::process_async_results<async_loaded_data>(r, [this](entt::entity e, async_loaded_data && loaded_data) {
				r.emplace<data::model_data>(e, std::move(loaded_data.model_data));
				apply_offset(e, loaded_data.offset_to_apply);
			});
		}

		void load_model(entt::registry &, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto & f = r.get<data::model>(e).file.c_str();
			if (std::filesystem::path(f).extension() != ".vox")
				return;

			kengine::start_async_task(
				r, e,
				data::async_task::string("magica_voxel: load %s", f),
				std::async(std::launch::async, [this, e, &f] {
					return load_model_data(e, f);
				})
			);
		}

		async_loaded_data load_model_data(entt::entity e, const char * file) noexcept {
			KENGINE_PROFILING_SCOPE;

			kengine_logf(r, log, "systems/magica_voxel", "Loading model %zu for %s", e, file);
			const putils::string<256> binary_file("%s.bin", file);

			if (!std::filesystem::exists(binary_file.c_str())) {
				kengine_log(r, log, "magica_voxel/load_model", "Binary file does not exist, creating it");
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

		void unserialize(const char * f, data::model_data::mesh & mesh_data, magica_voxel_format::chunk_content::size & size) noexcept {
			KENGINE_PROFILING_SCOPE;

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

		data::model_data::free_func release(entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			return [this, e] {
				if (const auto model_data = r.try_get<data::model_data>(e)) {
					delete[] (const char *)model_data->meshes[0].vertices.data;
					delete[] (const char *)model_data->meshes[0].indices.data;
				}
			};
		}

		model_and_offset load_vox_model(const char * f) noexcept {
			KENGINE_PROFILING_SCOPE;

			std::ifstream stream(f, std::ios::binary);
			if (!stream) {
				kengine_assert_failed(r, "[magica_voxel] Failed to load '", f, "'");
				return model_and_offset{};
			}

			check_header(stream);

			magica_voxel_format::chunk_header main;
			read_from_stream(main, stream);
			if (!id_matches(main.id, "MAIN")) {
				kengine_assert_failed(r, "[magica_voxel] Expected 'MAIN' chunk header in '", f, "'");
				return model_and_offset{};
			}

			magica_voxel_format::chunk_header first;
			read_from_stream(first, stream);
			if (!id_matches(first.id, "SIZE")) {
				kengine_assert_failed(r, "[magica_voxel] Expected 'SIZE' chunk header in '", f, "'");
				return model_and_offset{};
			}
			kengine_assert(r, first.children_bytes == 0);
			kengine_assert(r, first.content_bytes == sizeof(magica_voxel_format::chunk_content::size));

			magica_voxel_format::chunk_content::size size;
			read_from_stream(size, stream);

			PolyVox::RawVolume<data::polyvox::vertex_data> volume(PolyVox::Region{
				{ 0, 0, 0 },
				{ size.x, size.z, size.y },
			});

			magica_voxel_format::chunk_header voxels_header;
			read_from_stream(voxels_header, stream);
			if (!id_matches(voxels_header.id, "XYZI")) {
				kengine_assert_failed(r, "[magica_voxel] Expected 'XYZI' chunk header in '", f, "'");
				return model_and_offset{};
			}

			magica_voxel_format::chunk_content::rgba rgba;

			magica_voxel_format::chunk_content::xyzi xyzi;
			read_from_stream(xyzi, stream);
			kengine_assert(r, voxels_header.content_bytes == sizeof(xyzi) + xyzi.num_voxels * sizeof(int));

			for (int i = 0; i < xyzi.num_voxels; ++i) {
				magica_voxel_format::chunk_content::xyzi::voxel voxel;
				read_from_stream(voxel, stream);

				const auto & color = rgba.palette[voxel.color_index];
				const data::polyvox::vertex_data voxel_value{ { (float)color.r / 255.f, (float)color.g / 255.f, (float)color.b / 255.f } };
				volume.setVoxel(voxel.x, voxel.z, voxel.y, voxel_value);
			}

			return { build_mesh(std::move(volume)), size };
		}

		bool id_matches(const char * s1, const char * s2) noexcept {
			KENGINE_PROFILING_SCOPE;
			return strncmp(s1, s2, 4) == 0;
		}

		void check_header(std::istream & s) noexcept {
			KENGINE_PROFILING_SCOPE;

			magica_voxel_format::file_header header;
			read_from_stream(header, s);
			kengine_assert(r, id_matches(header.id, "VOX "));
			kengine_assert(r, header.version_number == 150);
		}

		data::model_data generate_model_data(entt::entity e, const mesh_type & mesh) noexcept {
			KENGINE_PROFILING_SCOPE;

			data::model_data model_data;
			model_data.free = release(e);
			model_data.init<mesh_type::VertexType>();

			auto & mesh_data = model_data.meshes.emplace_back();
			mesh_data.vertices = { mesh.getNoOfVertices(), sizeof(putils_typeof(mesh)::VertexType), mesh.getRawVertexData() };
			mesh_data.indices = { mesh.getNoOfIndices(), sizeof(putils_typeof(mesh)::IndexType), mesh.getRawIndexData() };
			mesh_data.index_type = putils::meta::type<putils_typeof(mesh)::IndexType>::index;

			return model_data;
		}

		void apply_offset(entt::entity e, const magica_voxel_format::chunk_content::size & size) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (r.all_of<data::transform>(e)) {
				kengine_logf(r, log, "magica_voxel/load_model", "%zu already has a data::transform. mesh offset will not be applied", e);
				return;
			}
			kengine_log(r, log, "magica_voxel/load_model", "Applying mesh offset");

			auto & box = r.get_or_emplace<data::transform>(e).bounding_box;
			box.position.x -= size.x / 2.f * box.size.x;
			box.position.z -= size.y / 2.f * box.size.z;
		}

		void serialize(const char * f, const data::model_data & model_data, const magica_voxel_format::chunk_content::size & size) noexcept {
			KENGINE_PROFILING_SCOPE;

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

	DEFINE_KENGINE_SYSTEM_CREATOR(magica_voxel)
}
