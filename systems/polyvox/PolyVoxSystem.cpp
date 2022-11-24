#include "PolyVoxSystem.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// polyvox
#include <PolyVox/CubicSurfaceExtractor.h>

// meta
#include "meta/type.hpp"

// putils
#include "forward_to.hpp"

// kengine data
#include "data/ModelDataComponent.hpp"
#include "data/PolyVoxComponent.hpp"
#include "data/TransformComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	static auto buildMesh(PolyVox::RawVolume<PolyVoxComponent::VertexData> && volume) noexcept {
		const auto encodedMesh = PolyVox::extractCubicMesh(&volume, volume.getEnclosingRegion());
		const auto mesh = PolyVox::decodeMesh(encodedMesh);
		return mesh;
	}

	struct PolyVoxSystem {
		entt::registry & r;

		PolyVoxSystem(entt::handle e) noexcept
			: r(*e.registry())
		{
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "PolyVoxSystem");

			e.emplace<functions::Execute>(putils_forward_to_this(execute));
		}

		void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Verbose, "Execute", "PolyVoxSystem");

			for (auto [e, poly] : r.view<PolyVoxComponent>().each()) {
				if (!poly.changed)
					continue;
				kengine_logf(r, Verbose, "Execute/PolyVoxSystem", "Rebuilding mesh for %zu", e);
				poly.changed = false;

				ModelDataComponent modelData;

				auto & mesh = r.emplace<PolyVoxMeshContainerComponent>(e).mesh;
				mesh = buildMesh(std::move(poly.volume));

				const auto & centre = poly.volume.getEnclosingRegion().getCentre();
				auto & model = r.get_or_emplace<TransformComponent>(e);
				model.boundingBox.position = { (float)centre.getX(), (float)centre.getY(), (float)centre.getZ() };

				ModelDataComponent::Mesh meshData;
				meshData.vertices = { mesh.getNoOfVertices(), sizeof(PolyVoxMeshContainerComponent::MeshType::VertexType), mesh.getRawVertexData() };
				meshData.indices = { mesh.getNoOfIndices(), sizeof(PolyVoxMeshContainerComponent::MeshType::IndexType), mesh.getRawIndexData() };
				meshData.indexType = putils::meta::type<PolyVoxMeshContainerComponent::MeshType::IndexType>::index;
				modelData.meshes.push_back(meshData);

				modelData.free = FreePolyVoxMeshData(e);
				modelData.init<PolyVoxMeshContainerComponent::MeshType::VertexType>();

				r.emplace<ModelDataComponent>(e, std::move(modelData));
			}
		}

		ModelDataComponent::FreeFunc FreePolyVoxMeshData(entt::entity e) noexcept {
			return [this, e]() noexcept {
				KENGINE_PROFILING_SCOPE;
				kengine_logf(r, Log, "PolyVoxSystem", "Releasing PolyVoxMeshContainer for %zu", e);
				auto & mesh = r.get_or_emplace<PolyVoxMeshContainerComponent>(e).mesh; // previous `attach` hasn't been processed yet, so `get` would assert
				mesh.clear();
				r.remove<PolyVoxMeshContainerComponent>(e);
			};
		}

		struct PolyVoxMeshContainerComponent {
			using MeshType = decltype(buildMesh(PolyVox::RawVolume<PolyVoxComponent::VertexData>({ {0, 0, 0 }, {0, 0, 0} })));
			MeshType mesh;
		};
	};

	void addPolyVoxSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<PolyVoxSystem>(e);
	}
}
