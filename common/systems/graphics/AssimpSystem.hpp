#pragma once

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "System.hpp"
#include "EntityManager.hpp"
#include "components/GraphicsComponent.hpp"
#include "components/ModelLoaderComponent.hpp"
#include "TexturedShader.hpp"

#include "file_extension.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace kengine {
	namespace AssImp {
		struct ModelEntity {
			struct Mesh {
				struct Vertex {
					float position[3];
					float normal[3];
					float texCoords[2];

					pmeta_get_attributes(
						pmeta_reflectible_attribute(&Vertex::position),
						pmeta_reflectible_attribute(&Vertex::normal),
						pmeta_reflectible_attribute(&Vertex::texCoords)
					);
				};

				std::vector<Vertex> vertices;
				std::vector<unsigned int> indices;
			};

			std::vector<Mesh> meshes;
			kengine::Entity::ID id;
		};

		static std::unordered_map<putils::string<64>, ModelEntity> models;

		static unsigned int textureFromFile(const char * file, const char * directory) {
			static std::unordered_map<putils::string<64>, unsigned int> textures;

			putils::string<64> fullPath("%s/%s", directory, file);

			{
				const auto it = textures.find(fullPath);
				if (it != textures.end())
					return it->second;
			}

			unsigned int textureID;
			glGenTextures(1, &textureID);
			int width, height, components;
			unsigned char * data = stbi_load(fullPath, &width, &height, &components, 0);
			if (data != nullptr) {
				GLenum format;

				switch (components) {
				case 1:
					format = GL_RED;
					break;
				case 3:
					format = GL_RGB;
					break;
				case 4:
					format = GL_RGBA;
					break;
				default:
					assert(false);
				}

				glBindTexture(GL_TEXTURE_2D, textureID);
				glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				stbi_image_free(data);
			}

			textures[fullPath] = textureID;
			return textureID;
		}

		static void loadMaterialTextures(std::vector<unsigned int> & textures, const char * directory, const aiMaterial * mat, aiTextureType type) {
			for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
				aiString path;
				mat->GetTexture(type, i, &path);
				textures.push_back(textureFromFile(path.C_Str(), directory));
			}
		}

		static ModelEntity::Mesh processMesh(kengine::ModelInfoTexturesComponent & textures, const char * directory, const aiMesh * mesh, const aiScene * scene) {
			ModelEntity::Mesh ret;

			for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
				ModelEntity::Mesh::Vertex vertex;

				vertex.position[0] = mesh->mVertices[i].x;
				vertex.position[1] = mesh->mVertices[i].y;
				vertex.position[2] = mesh->mVertices[i].z;

				vertex.normal[0] = mesh->mNormals[i].x;
				vertex.normal[1] = mesh->mNormals[i].y;
				vertex.normal[2] = mesh->mNormals[i].z;

				if (mesh->mTextureCoords[0] != nullptr) {
					vertex.texCoords[0] = mesh->mTextureCoords[0][i].x;
					vertex.texCoords[1] = mesh->mTextureCoords[0][i].y;
				} else {
					vertex.texCoords[0] = 0.f;
					vertex.texCoords[1] = 0.f;
				}

				ret.vertices.push_back(vertex);
			}

			for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
				for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
					ret.indices.push_back(mesh->mFaces[i].mIndices[j]);

			kengine::ModelInfoTexturesComponent::MeshTextures meshTextures;
			if (mesh->mMaterialIndex >= 0) {
				const auto material = scene->mMaterials[mesh->mMaterialIndex];
				loadMaterialTextures(meshTextures.diffuse, directory, material, aiTextureType_DIFFUSE);
				loadMaterialTextures(meshTextures.specular, directory, material, aiTextureType_SPECULAR);
			} else
				assert(false);
			textures.meshes.push_back(std::move(meshTextures));

			return ret;
		}

		static void processNode(ModelEntity & modelData, kengine::ModelInfoTexturesComponent & textures, const char * directory, const aiNode * node, const aiScene * scene) {
			for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
				const aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
				modelData.meshes.push_back(processMesh(textures, directory, mesh, scene));
			}

			for (unsigned int i = 0; i < node->mNumChildren; ++i)
				processNode(modelData, textures, directory, node->mChildren[i], scene);
		}

		auto loadFile(const char * file, ModelEntity & model, kengine::ModelInfoTexturesComponent & textures) {
			const putils::string<64> f(file);

			return [f, &model, &textures] {
				Assimp::Importer importer;
				const aiScene * scene = importer.ReadFile(f.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_OptimizeMeshes);
				if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
					std::cerr << importer.GetErrorString() << '\n';
					assert(false);
				}

				const auto dir = putils::get_directory<64>(f.begin());

				processNode(model, textures, dir.c_str(), scene->mRootNode, scene);

				kengine::ModelLoaderComponent::ModelData ret;
				for (const auto & mesh : model.meshes) {
					decltype(ret)::MeshData meshData;
					meshData.vertices = { mesh.vertices.size(), sizeof(ModelEntity::Mesh::Vertex), mesh.vertices.data() };
					meshData.indices = { mesh.indices.size(), sizeof(mesh.indices[0]), mesh.indices.data() };
					meshData.indexType = GL_UNSIGNED_INT;
					ret.meshes.push_back(meshData);
				}
				return ret;
			};
		}
	}

	class AssImpSystem : public kengine::System<AssImpSystem, kengine::packets::RegisterEntity> {
	public:
		AssImpSystem(kengine::EntityManager & em) : System(em), _em(em) {
			onLoad("");
		}

		void onLoad(const char *) noexcept override {
			_em += [this](kengine::Entity & e) {
				e += kengine::makeGBufferShaderComponent<TexturedShader>(_em);
			};
		}

		void handle(kengine::packets::RegisterEntity p) {
			if (!p.e.has<kengine::GraphicsComponent>())
				return;

			auto & graphics = p.e.get<kengine::GraphicsComponent>();
			const auto & layer = graphics.getLayer("main");
			const auto & file = layer.appearance;

			Assimp::Importer importer;
			if (!importer.IsExtensionSupported(putils::file_extension(file.c_str())))
				return;

			p.e += TexturedModelComponent{};

			const auto it = AssImp::models.find(file);
			if (it != AssImp::models.end()) {
				p.e += kengine::ModelComponent{ it->second.id };
				return;
			}

			auto & modelData = AssImp::models[file];
			_em += [&modelData, file](kengine::Entity & e) {
				modelData.id = e.id;
				auto & textures = e.attach<kengine::ModelInfoTexturesComponent>();
				e += kengine::ModelLoaderComponent{
					AssImp::loadFile(file.c_str(), modelData, textures),
					[](putils::gl::Program & p) { p.setVertexType<AssImp::ModelEntity::Mesh::Vertex>(); }
				};
			};

			p.e += kengine::ModelComponent{ modelData.id };
		}

	private:
		kengine::EntityManager & _em;
	};
}
