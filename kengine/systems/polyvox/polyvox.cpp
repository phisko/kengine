#include "polyvox.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// polyvox
#include <PolyVox/CubicSurfaceExtractor.h>

// meta
#include "putils/meta/type.hpp"

// putils
#include "putils/forward_to.hpp"

// kengine data
#include "kengine/data/model_data.hpp"
#include "kengine/data/polyvox.hpp"
#include "kengine/data/transform.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::systems {
	static auto build_mesh(PolyVox::RawVolume<data::polyvox::vertex_data> && volume) noexcept {
		const auto encoded_mesh = PolyVox::extractCubicMesh(&volume, volume.getEnclosingRegion());
		const auto mesh = PolyVox::decodeMesh(encoded_mesh);
		return mesh;
	}

	struct polyvox {
		entt::registry & r;

		polyvox(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "polyvox", "Initializing");

			e.emplace<functions::execute>(putils_forward_to_this(execute));
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "polyvox", "Executing");

			for (auto [e, poly] : r.view<data::polyvox>().each()) {
				if (!poly.changed) {
					kengine_logf(r, very_verbose, "polyvox", "Polyvox component in [%u] hasn't changed", e);
					continue;
				}

				kengine_logf(r, log, "polyvox", "Rebuilding mesh for [%u]", e);
				poly.changed = false;

				data::model_data model_data;

				auto & mesh = r.emplace<mesh_container>(e).mesh;
				mesh = build_mesh(std::move(poly.volume));

				const auto & centre = poly.volume.getEnclosingRegion().getCentre();
				auto & model = r.get_or_emplace<data::transform>(e);
				model.bounding_box.position = { (float)centre.getX(), (float)centre.getY(), (float)centre.getZ() };

				data::model_data::mesh mesh_data;
				mesh_data.vertices = { mesh.getNoOfVertices(), sizeof(mesh_container::mesh_type::VertexType), mesh.getRawVertexData() };
				mesh_data.indices = { mesh.getNoOfIndices(), sizeof(mesh_container::mesh_type::IndexType), mesh.getRawIndexData() };
				mesh_data.index_type = putils::meta::type<mesh_container::mesh_type::IndexType>::index;
				model_data.meshes.push_back(mesh_data);

				model_data.free = free_polyvox_mesh_data(e);
				model_data.init<mesh_container::mesh_type::VertexType>();

				r.emplace<data::model_data>(e, std::move(model_data));
			}
		}

		data::model_data::free_func free_polyvox_mesh_data(entt::entity e) noexcept {
			return [this, e]() noexcept {
				KENGINE_PROFILING_SCOPE;
				if (!r.all_of<mesh_container>(e))
					return;
				kengine_logf(r, log, "polyvox", "Releasing mesh for [%u]", e);
				auto & mesh = r.get<mesh_container>(e).mesh;
				mesh.clear();
				r.remove<mesh_container>(e);
			};
		}

		struct mesh_container {
			using mesh_type = decltype(build_mesh(PolyVox::RawVolume<data::polyvox::vertex_data>({ { 0, 0, 0 }, { 0, 0, 0 } })));
			mesh_type mesh;
		};
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		polyvox,
		polyvox::mesh_container
	)
}
