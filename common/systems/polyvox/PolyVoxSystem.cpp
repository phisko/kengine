#include "PolyVoxSystem.hpp"

#include <PolyVox/CubicSurfaceExtractor.h>

#include "EntityManager.hpp"
#include "PolyVoxShader.hpp"
#include "components/ShaderComponent.hpp"
#include "components/ModelLoaderComponent.hpp"
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
		pmeta_get_class_name(PolyVoxMeshContainerComponent);
	};

	static auto ExtractPolyVoxMeshData(kengine::Entity::ID id, kengine::EntityManager & em) {
		return [id, &em] {
			auto & e = em.getEntity(id);

			auto & poly = e.get<PolyVoxComponent>();
			auto & mesh = e.attach<PolyVoxMeshContainerComponent>().mesh;

			mesh = detailPolyVox::buildMesh(poly.volume);

			const auto & centre = poly.volume.getEnclosingRegion().getCentre();
			auto & model = e.attach<ModelComponent>();
			model.boundingBox.position = { (float)centre.getX(), (float)centre.getY(), (float)centre.getZ() };

			kengine::ModelLoaderComponent::ModelData ret;

			kengine::ModelLoaderComponent::ModelData::MeshData meshData;
			meshData.vertices = { mesh.getNoOfVertices(), sizeof(PolyVoxMeshContainerComponent::MeshType::VertexType), mesh.getRawVertexData() };
			meshData.indices = { mesh.getNoOfIndices(), sizeof(PolyVoxMeshContainerComponent::MeshType::IndexType), mesh.getRawIndexData() };
			meshData.indexType = sizeof(PolyVoxMeshContainerComponent::MeshType::IndexType) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
			ret.meshes.push_back(meshData);

			return ret;
		};
	}

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

			e += kengine::ModelLoaderComponent{
				ExtractPolyVoxMeshData(e.id, _em),
				FreePolyVoxMeshData(e.id, _em),
				[]() { putils::gl::setPolyvoxVertexType<PolyVoxMeshContainerComponent::MeshType::VertexType>(); }
			};

			poly.changed = false;
		}
	}
}
