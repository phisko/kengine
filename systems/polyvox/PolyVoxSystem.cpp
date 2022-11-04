#include "PolyVoxSystem.hpp"
#include "kengine.hpp"

// polyvox
#include <PolyVox/CubicSurfaceExtractor.h>

// meta
#include "meta/type.hpp"

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
		static void init(Entity & e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Log, "Init", "PolyVoxSystem");
			e += functions::Execute{ execute };
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Verbose, "Execute", "PolyVoxSystem");

			for (auto [e, poly] : entities.with<PolyVoxComponent>()) {
				if (!poly.changed)
					continue;
				kengine_logf(Verbose, "Execute/PolyVoxSystem", "Rebuilding mesh for %zu", e.id);
				poly.changed = false;

				ModelDataComponent modelData;

				auto & mesh = e.attach<PolyVoxMeshContainerComponent>().mesh;
				mesh = buildMesh(std::move(poly.volume));

				const auto & centre = poly.volume.getEnclosingRegion().getCentre();
				auto & model = e.attach<TransformComponent>();
				model.boundingBox.position = { (float)centre.getX(), (float)centre.getY(), (float)centre.getZ() };

				ModelDataComponent::Mesh meshData;
				meshData.vertices = { mesh.getNoOfVertices(), sizeof(PolyVoxMeshContainerComponent::MeshType::VertexType), mesh.getRawVertexData() };
				meshData.indices = { mesh.getNoOfIndices(), sizeof(PolyVoxMeshContainerComponent::MeshType::IndexType), mesh.getRawIndexData() };
				meshData.indexType = putils::meta::type<PolyVoxMeshContainerComponent::MeshType::IndexType>::index;
				modelData.meshes.push_back(meshData);

				modelData.free = FreePolyVoxMeshData(e.id);
				modelData.init<PolyVoxMeshContainerComponent::MeshType::VertexType>();

				e += std::move(modelData);
			}
		}

		static ModelDataComponent::FreeFunc FreePolyVoxMeshData(EntityID id) noexcept {
			return [id]() noexcept {
				KENGINE_PROFILING_SCOPE;
				kengine_logf(Log, "PolyVoxSystem", "Releasing PolyVoxMeshContainer for %zu", id);
				auto e = entities[id];
				auto & mesh = e.attach<PolyVoxMeshContainerComponent>().mesh; // previous `attach` hasn't been processed yet, so `get` would assert
				mesh.clear();
				e.detach<PolyVoxMeshContainerComponent>();
			};
		}

		struct PolyVoxMeshContainerComponent {
			using MeshType = decltype(buildMesh(PolyVox::RawVolume<PolyVoxComponent::VertexData>({ {0, 0, 0 }, {0, 0, 0} })));
			MeshType mesh;
		};
	};

	EntityCreator * PolyVoxSystem() noexcept {
		return PolyVoxSystem::init;
	}
}
