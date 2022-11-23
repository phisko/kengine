#include "PolyVoxSystem.hpp"

// entt
#include <entt/entity/registry.hpp>

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
		static void init(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "PolyVoxSystem");

			_r = &r;

			const auto e = r.create();
			r.emplace<functions::Execute>(e, execute);
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(*_r, Verbose, "Execute", "PolyVoxSystem");

			for (auto [e, poly] : _r->view<PolyVoxComponent>().each()) {
				if (!poly.changed)
					continue;
				kengine_logf(*_r, Verbose, "Execute/PolyVoxSystem", "Rebuilding mesh for %zu", e);
				poly.changed = false;

				ModelDataComponent modelData;

				auto & mesh = _r->emplace<PolyVoxMeshContainerComponent>(e).mesh;
				mesh = buildMesh(std::move(poly.volume));

				const auto & centre = poly.volume.getEnclosingRegion().getCentre();
				auto & model = _r->get_or_emplace<TransformComponent>(e);
				model.boundingBox.position = { (float)centre.getX(), (float)centre.getY(), (float)centre.getZ() };

				ModelDataComponent::Mesh meshData;
				meshData.vertices = { mesh.getNoOfVertices(), sizeof(PolyVoxMeshContainerComponent::MeshType::VertexType), mesh.getRawVertexData() };
				meshData.indices = { mesh.getNoOfIndices(), sizeof(PolyVoxMeshContainerComponent::MeshType::IndexType), mesh.getRawIndexData() };
				meshData.indexType = putils::meta::type<PolyVoxMeshContainerComponent::MeshType::IndexType>::index;
				modelData.meshes.push_back(meshData);

				modelData.free = FreePolyVoxMeshData(e);
				modelData.init<PolyVoxMeshContainerComponent::MeshType::VertexType>();

				_r->emplace<ModelDataComponent>(e, std::move(modelData));
			}
		}

		static ModelDataComponent::FreeFunc FreePolyVoxMeshData(entt::entity e) noexcept {
			return [e]() noexcept {
				KENGINE_PROFILING_SCOPE;
				kengine_logf(*_r, Log, "PolyVoxSystem", "Releasing PolyVoxMeshContainer for %zu", e);
				auto & mesh = _r->get_or_emplace<PolyVoxMeshContainerComponent>(e).mesh; // previous `attach` hasn't been processed yet, so `get` would assert
				mesh.clear();
				_r->remove<PolyVoxMeshContainerComponent>(e);
			};
		}

		struct PolyVoxMeshContainerComponent {
			using MeshType = decltype(buildMesh(PolyVox::RawVolume<PolyVoxComponent::VertexData>({ {0, 0, 0 }, {0, 0, 0} })));
			MeshType mesh;
		};

		static inline entt::registry * _r;
	};

	void PolyVoxSystem(entt::registry & r) noexcept {
		PolyVoxSystem::init(r);
	}
}
