#include "PolyVoxSystem.hpp"

#include <PolyVox/CubicSurfaceExtractor.h>

#include "EntityManager.hpp"
#include "PolyVoxShader.hpp"
#include "components/ShaderComponent.hpp"
#include "components/ModelDataComponent.hpp"
#include "components/PolyVoxComponent.hpp"
#include "components/ModelComponent.hpp"

namespace kengine {
	namespace detailPolyVox {
		static auto buildMesh(PolyVox::RawVolume<PolyVoxComponent::VertexData> & volume) {
			const auto encodedMesh = PolyVox::extractCubicMesh(&volume, volume.getEnclosingRegion());
			const auto mesh = PolyVox::decodeMesh(encodedMesh);
			return mesh;
		}
	}

	struct PolyVoxMeshContainerComponent {
		using MeshType = decltype(detailPolyVox::buildMesh(PolyVox::RawVolume<PolyVoxComponent::VertexData>({ {0, 0, 0 }, {0, 0, 0} })));
		MeshType mesh;
	};

	static auto FreePolyVoxMeshData(Entity::ID id, EntityManager & em) {
		return [id, &em] {
			auto & e = em.getEntity(id);
			auto & mesh = e.attach<PolyVoxMeshContainerComponent>().mesh; // previous `attach` hasn't been processed yet, so `get` would assert
			mesh.clear();
			e.detach<PolyVoxMeshContainerComponent>();
		};
	}

	PolyVoxSystem::PolyVoxSystem(kengine::EntityManager & em) : System(em), _em(em) {
		_em += [this](kengine::Entity & e) {
			e += kengine::makeGBufferShaderComponent<PolyVoxShader>(_em);
		};
	}

	void PolyVoxSystem::execute() noexcept {
		for (auto &[e, poly] : _em.getEntities<PolyVoxComponent>()) {
			if (!poly.changed)
				continue;
			poly.changed = false;

			ModelDataComponent modelData;

			auto & mesh = e.attach<PolyVoxMeshContainerComponent>().mesh;
			mesh = detailPolyVox::buildMesh(poly.volume);

			const auto & centre = poly.volume.getEnclosingRegion().getCentre();
			auto & model = e.attach<ModelComponent>();
			model.boundingBox.position = { (float)centre.getX(), (float)centre.getY(), (float)centre.getZ() };

			ModelDataComponent::Mesh meshData;
			meshData.vertices = { mesh.getNoOfVertices(), sizeof(PolyVoxMeshContainerComponent::MeshType::VertexType), mesh.getRawVertexData() };
			meshData.indices = { mesh.getNoOfIndices(), sizeof(PolyVoxMeshContainerComponent::MeshType::IndexType), mesh.getRawIndexData() };
			meshData.indexType = sizeof(PolyVoxMeshContainerComponent::MeshType::IndexType) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
			modelData.meshes.push_back(meshData);

			modelData.free = FreePolyVoxMeshData(e.id, _em);
			modelData.vertexRegisterFunc =putils::gl::setPolyvoxVertexType<PolyVoxMeshContainerComponent::MeshType::VertexType>; 

			e += std::move(modelData);
		}
	}
}
