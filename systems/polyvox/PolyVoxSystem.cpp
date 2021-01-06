#include "PolyVoxSystem.hpp"

#include <PolyVox/CubicSurfaceExtractor.h>

#include "kengine.hpp"
#include "PolyVoxShader.hpp"
#include "data/ShaderComponent.hpp"
#include "data/ModelDataComponent.hpp"
#include "data/PolyVoxComponent.hpp"
#include "data/ModelComponent.hpp"

#include "functions/Execute.hpp"

#include "meta/type.hpp"

namespace kengine {
	static auto buildMesh(PolyVox::RawVolume<PolyVoxComponent::VertexData> && volume) noexcept {
		const auto encodedMesh = PolyVox::extractCubicMesh(&volume, volume.getEnclosingRegion());
		const auto mesh = PolyVox::decodeMesh(encodedMesh);
		return mesh;
	}

	EntityCreator * PolyVoxSystem() noexcept {
		struct PolyVoxMeshContainerComponent {
			using MeshType = decltype(buildMesh(PolyVox::RawVolume<PolyVoxComponent::VertexData>({ {0, 0, 0 }, {0, 0, 0} })));
			MeshType mesh;
		};

		struct impl {
			static void init(Entity & e) noexcept {
				e += functions::Execute{ execute };
				e += opengl::ShaderComponent{ std::make_unique<PolyVoxShader>() };
				e += opengl::GBufferShaderComponent{};
			}

			static void execute(float deltaTime) noexcept {
				for (auto [e, poly] : entities.with<PolyVoxComponent>()) {
					if (!poly.changed)
						continue;
					poly.changed = false;

					ModelDataComponent modelData;

					auto & mesh = e.attach<PolyVoxMeshContainerComponent>().mesh;
					mesh = buildMesh(std::move(poly.volume));

					const auto & centre = poly.volume.getEnclosingRegion().getCentre();
					auto & model = e.attach<ModelComponent>();
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
					auto e = entities.get(id);
					auto & mesh = e.attach<PolyVoxMeshContainerComponent>().mesh; // previous `attach` hasn't been processed yet, so `get` would assert
					mesh.clear();
					e.detach<PolyVoxMeshContainerComponent>();
				};
			}

		};

		return [](Entity & e) noexcept {
			impl::init(e);
		};
	}
}
