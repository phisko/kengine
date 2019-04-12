#pragma once

#include <PolyVox/CubicSurfaceExtractor.h>

#include "System.hpp"
#include "EntityManager.hpp"
#include "PolyVoxShader.hpp"
#include "components/ShaderComponent.hpp"
#include "components/ModelLoaderComponent.hpp"

namespace kengine {
	namespace detailPolyVox {
		static auto buildMesh(PolyVox::RawVolume<PolyVoxComponent::VertexData> & volume) {
			const auto encodedMesh = PolyVox::extractCubicMesh(&volume, volume.getEnclosingRegion());
			const auto mesh = PolyVox::decodeMesh(encodedMesh);
			return mesh;
		}
	}

	class PolyVoxSystem : public kengine::System<PolyVoxSystem> {
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

				kengine::ModelLoaderComponent::ModelData ret;

				kengine::ModelLoaderComponent::ModelData::MeshData meshData;
				meshData.vertices = { mesh.getNoOfVertices(), sizeof(PolyVoxMeshContainerComponent::MeshType::VertexType), mesh.getRawVertexData() };
				meshData.indices = { mesh.getNoOfIndices(), sizeof(PolyVoxMeshContainerComponent::MeshType::IndexType), mesh.getRawIndexData() };
				meshData.indexType = sizeof(PolyVoxMeshContainerComponent::MeshType::IndexType) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
				ret.meshes.push_back(meshData);

				const auto & centre = poly.volume.getEnclosingRegion().getCentre();
				ret.offsetToCentre = { (float)centre.getX(), (float)centre.getY(), (float)centre.getZ() };
				return ret;
			};
		}


	public:
		PolyVoxSystem(kengine::EntityManager & em) : System(em), _em(em) {
			onLoad("");
		}

		void onLoad(const char *) noexcept override {
			_em += [this](kengine::Entity & e) {
				e += kengine::makeGBufferShaderComponent<PolyVoxShader>(_em);
			};
		}

		void execute() noexcept override {
			for (auto &[e, poly] : _em.getEntities<PolyVoxComponent>()) {
				if (!poly.changed)
					continue;

				e += kengine::ModelLoaderComponent{
					ExtractPolyVoxMeshData(e.id, _em),
					[]() { putils::gl::setPolyvoxVertexType<PolyVoxMeshContainerComponent::MeshType::VertexType>(); }
				};

				poly.changed = false;
			}
		}

	private:
		kengine::EntityManager & _em;
	};
}
