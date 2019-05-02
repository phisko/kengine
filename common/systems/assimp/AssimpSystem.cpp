#include "AssimpSystem.hpp"
#include "EntityManager.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "file_extension.hpp"
#include "imgui.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "TexturedShader.hpp"
#include "AssImpShadowMap.hpp"
#include "AssImpShadowCube.hpp"

#include "components/GraphicsComponent.hpp"
#include "components/ModelLoaderComponent.hpp"
#include "components/ModelInfoComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/TexturedModelComponent.hpp"

#include "components/AnimationComponent.hpp"
#include "components/SkeletonComponent.hpp"

#include "components/ShaderComponent.hpp"
#include "components/ImGuiComponent.hpp"

#include "AssImpDefines.h"

namespace kengine {
	namespace AssImp {
		static aiMatrix4x4 toAiMat(const glm::mat4 & mat) {
			return aiMatrix4x4(mat[0][0], mat[1][0], mat[2][0], mat[3][0],
				mat[0][1], mat[1][1], mat[2][1], mat[3][1],
				mat[0][2], mat[1][2], mat[2][2], mat[3][2],
				mat[0][3], mat[1][3], mat[2][3], mat[3][3]);
		}

		static glm::mat4 toglm(const aiMatrix4x4 & mat) {
			return glm::make_mat4(&mat.a1);
		}

		static glm::mat4 toglmWeird(const aiMatrix4x4 & mat) {
			return glm::transpose(toglm(mat));
		}

		static glm::vec3 toglm(const aiVector3D & vec) { return { vec.x, vec.y, vec.z }; }

		static glm::quat toglm(const aiQuaternion & quat) { return { quat.w, quat.x, quat.y, quat.z }; }

		struct ModelEntity {
			struct Mesh {
				struct Vertex {
					float position[3];
					float normal[3];
					float texCoords[2];

					float boneWeights[KENGINE_ASSIMP_BONE_INFO_PER_VERTEX] = { 0.f };
					unsigned int boneIDs[KENGINE_ASSIMP_BONE_INFO_PER_VERTEX] = { 0 };

					pmeta_get_attributes(
						pmeta_reflectible_attribute(&Vertex::position),
						pmeta_reflectible_attribute(&Vertex::normal),
						pmeta_reflectible_attribute(&Vertex::texCoords),

						pmeta_reflectible_attribute(&Vertex::boneWeights),
						pmeta_reflectible_attribute(&Vertex::boneIDs)
					);
				};

				std::vector<Vertex> vertices;
				std::vector<unsigned int> indices;
			};

			Assimp::Importer importer;
			std::vector<Mesh> meshes;
			float pitch;
			float yaw;
			putils::Vector3f offset;
			putils::Vector3f scale;
			kengine::Entity::ID id;
		};

		struct AssImpSkeletonComponent : kengine::not_serializable {
			struct Mesh {
				struct Bone {
					putils::string<KENGINE_ASSIMP_BONE_NAME_MAX_LENGTH> name;
					aiNode * node = nullptr;
					std::vector<const aiNodeAnim *> animNodes;
					glm::mat4 offset;
				};
				putils::vector<Bone, KENGINE_SKELETON_MAX_BONES> bones;
			};

			std::vector<Mesh> meshes;
			glm::mat4 globalInverseTransform;

			pmeta_get_class_name(AssImpSkeletonComponent);
		};

		template<typename T>
		static unsigned int findPreviousIndex(T * arr, unsigned int size, float time) {
			for (unsigned i = 0; i < size - 1; ++i) {
				if (time < (float)arr[i + 1].mTime)
					return i;
			}
			return 0;
		}

		template<typename T, typename Func>
		static auto calculateInterpolatedValue(T * arr, unsigned int size, float time, Func func) {
			if (size == 1)
				return toglm(arr[0].mValue);

			const auto index = findPreviousIndex(arr, size, time);
			const auto & value = arr[index];
			const auto & nextValue = arr[index + 1];

			const auto deltaTime = (float)nextValue.mTime - (float)value.mTime;
			const auto factor = (time - (float)value.mTime) / (float)deltaTime;

			const auto startValue = toglm(value.mValue);
			const auto endValue = toglm(nextValue.mValue);

			return func(startValue, endValue, factor);
		}

		static glm::vec3 calculateInterpolatedPosition(const AssImpSkeletonComponent::Mesh::Bone & bone, float time, int currentAnim) {
			return calculateInterpolatedValue(bone.animNodes[currentAnim]->mPositionKeys, bone.animNodes[currentAnim]->mNumPositionKeys, time, [](const glm::vec3 & v1, const glm::vec3 & v2, float f) { return glm::mix(v1, v2, f); });
		}

		static glm::quat calculateInterpolatedRotation(const AssImpSkeletonComponent::Mesh::Bone & bone, float time, int currentAnim) {
			return calculateInterpolatedValue(bone.animNodes[currentAnim]->mRotationKeys, bone.animNodes[currentAnim]->mNumRotationKeys, time, glm::slerp<float, glm::defaultp>);
		}

		static glm::vec3 calculateInterpolatedScale(const AssImpSkeletonComponent::Mesh::Bone & bone, float time, int currentAnim) {
			return calculateInterpolatedValue(bone.animNodes[currentAnim]->mScalingKeys, bone.animNodes[currentAnim]->mNumScalingKeys, time, [](const glm::vec3 & v1, const glm::vec3 & v2, float f) { return glm::mix(v1, v2, f); });
		}

		static void updateKeyframeTransform(AssImpSkeletonComponent::Mesh::Bone & bone, float time, int currentAnim) {
			if (bone.animNodes[currentAnim] == nullptr) {
				bone.node->mTransformation = toAiMat(glm::mat4(1.f));
				return;
			}

			const auto pos = calculateInterpolatedPosition(bone, time, currentAnim);
			const auto rot = calculateInterpolatedRotation(bone, time, currentAnim);
			const auto scale = calculateInterpolatedScale(bone, time, currentAnim);

			glm::mat4 mat(1.f);
			mat = glm::translate(mat, pos);
			mat *= glm::mat4_cast(rot);
			mat = glm::scale(mat, scale);

			bone.node->mTransformation = toAiMat(mat);
		}

		static glm::mat4 getTransformWithParents(const AssImpSkeletonComponent::Mesh::Bone & bone) {
			glm::mat4 totalTransform(1.f);

			putils::vector<glm::mat4, KENGINE_ASSIMP_BONE_MAX_PARENTS> mats;
			for (auto node = bone.node; node != nullptr; node = node->mParent)
				mats.push_back(toglmWeird(node->mTransformation));

			for (int i = mats.size() - 1; i >= 0; --i)
				totalTransform *= mats[i];
			return totalTransform;
		}

		static void updateBoneMats(const AssImpSkeletonComponent::Mesh & skeleton, SkeletonComponent::Mesh & comp, const glm::mat4 & globalInverseTransform) {
			comp.boneMats.clear();
			for (unsigned int i = 0; i < KENGINE_SKELETON_MAX_BONES; ++i) {
				if (i >= skeleton.bones.size()) {
					comp.boneMats.push_back(glm::mat4(1.f));
					continue;
				}

				const auto & bone = skeleton.bones[i];
				const glm::mat4 transform = getTransformWithParents(bone);
				comp.boneMats.push_back(/*globalInverseTransform **/ transform * bone.offset);
			}
		}

		static std::unordered_map<putils::string<KENGINE_ASSIMP_MODEL_PATH_MAX_LENGTH>, ModelEntity> models;

		static unsigned int textureFromFile(const char * file, const char * directory) {
			static std::unordered_map<putils::string<KENGINE_ASSIMP_TEXTURE_PATH_MAX_LENGTH>, unsigned int> textures;

			const putils::string<KENGINE_ASSIMP_TEXTURE_PATH_MAX_LENGTH> fullPath("%s/%s", directory, file);

			{
				const auto it = textures.find(fullPath);
				if (it != textures.end())
					return it->second;
			}

			unsigned int textureID;
			glGenTextures(1, &textureID);
			int width, height, components;
			unsigned char * data = stbi_load(fullPath.c_str(), &width, &height, &components, 0);
			assert(data != nullptr);
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
				}
				else {
					vertex.texCoords[0] = 0.f;
					vertex.texCoords[1] = 0.f;
				}

				ret.vertices.push_back(vertex);
			}

			// for each bone
			for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
				const auto bone = mesh->mBones[i];
				// for each weight (vertex it has an influence on)
				for (unsigned int j = 0; j < bone->mNumWeights; ++j) {
					const auto & weight = bone->mWeights[j];
					auto & vertex = ret.vertices[weight.mVertexId];

					// add this bone to the vertex
#ifndef NDEBUG
					bool found = false;
#endif
					for (unsigned int k = 0; k < KENGINE_ASSIMP_BONE_INFO_PER_VERTEX; ++k)
						if (vertex.boneWeights[k] == 0.f) {
#ifndef NDEBUG
							found = true;
#endif
							vertex.boneWeights[k] = weight.mWeight;
							vertex.boneIDs[k] = i;
							break;
						}
#ifndef NDEBUG
					assert(found); // too many bones have info for a single vertex
#endif
				}
			}

			// For models with no skeleton
			for (auto & vertex : ret.vertices)
				if (vertex.boneWeights[0] == 0.f) {
					vertex.boneWeights[0] = 1.f;
					vertex.boneIDs[0] = 0;
				}

			for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
				for (unsigned int j = 0; j < mesh->mFaces[i].mNumIndices; ++j)
					ret.indices.push_back(mesh->mFaces[i].mIndices[j]);

			kengine::ModelInfoTexturesComponent::MeshTextures meshTextures;
			if (mesh->mMaterialIndex >= 0) {
				const auto material = scene->mMaterials[mesh->mMaterialIndex];
				loadMaterialTextures(meshTextures.diffuse, directory, material, aiTextureType_DIFFUSE);
				loadMaterialTextures(meshTextures.specular, directory, material, aiTextureType_SPECULAR);
			}
			else
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

		static void addNode(std::vector<aiNode *> & allNodes, aiNode * node) {
			allNodes.push_back(node);
			for (unsigned int i = 0; i < node->mNumChildren; ++i)
				addNode(allNodes, node->mChildren[i]);
		}

		static const AssImpSkeletonComponent::Mesh::Bone * findBone(const AssImpSkeletonComponent::Mesh & skeleton, const char * name) {
			for (const auto & bone : skeleton.bones)
				if (bone.name == name)
					return &bone;
			return nullptr;
		}

		static aiNode * findNode(const std::vector<aiNode *> & allNodes, const char * name) {
			for (const auto node : allNodes)
				if (strcmp(node->mName.data, name) == 0)
					return node;
			assert(false);
			return nullptr;
		}

		static aiNodeAnim * findNodeAnim(const std::vector<aiNodeAnim *> & allNodes, const char * name) {
			for (const auto node : allNodes)
				if (strcmp(node->mNodeName.data, name) == 0)
					return node;
			return nullptr;
		}

		static void addAnim(aiAnimation * aiAnim, AssImpSkeletonComponent & skeleton, AnimListComponent & animList) {
			AnimListComponent::Anim anim;
			anim.name = aiAnim->mName.data;
			anim.ticksPerSecond = (float)(aiAnim->mTicksPerSecond != 0 ? aiAnim->mTicksPerSecond : 25.0);
			anim.totalTime = (float)aiAnim->mDuration / anim.ticksPerSecond;
			animList.allAnims.push_back(anim);

			std::vector<aiNodeAnim *> allNodeAnims;
			for (unsigned int i = 0; i < aiAnim->mNumChannels; ++i)
				allNodeAnims.push_back(aiAnim->mChannels[i]);

			for (auto & mesh : skeleton.meshes)
				for (auto & bone : mesh.bones)
					bone.animNodes.push_back(findNodeAnim(allNodeAnims, bone.name));
		}

		auto loadFile(const char * file, kengine::EntityManager & em) {
			const putils::string<KENGINE_ASSIMP_MODEL_PATH_MAX_LENGTH> f(file);

			return [f, &em] {
				auto & model = models[f];

				auto & e = em.getEntity(model.id);
				auto & textures = e.attach<kengine::ModelInfoTexturesComponent>();
				auto & skeleton = e.attach<AssImp::AssImpSkeletonComponent>();
				auto & animList = e.attach<AnimListComponent>();

				const auto scene = model.importer.ReadFile(f.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals /*| aiProcess_OptimizeMeshes*/ | aiProcess_JoinIdenticalVertices);
				if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
					std::cerr << model.importer.GetErrorString() << '\n';
					assert(false);
				}

				const auto dir = putils::get_directory<64>(f.begin());
				processNode(model, textures, dir.c_str(), scene->mRootNode, scene);

				std::vector<aiNode *> allNodes;
				addNode(allNodes, scene->mRootNode);

				for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
					const auto mesh = scene->mMeshes[i];

					AssImpSkeletonComponent::Mesh meshBones;
					for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
						const auto aiBone = mesh->mBones[i];
						AssImpSkeletonComponent::Mesh::Bone bone;
						bone.name = aiBone->mName.data;
						bone.node = findNode(allNodes, bone.name);
						bone.offset = toglmWeird(aiBone->mOffsetMatrix);
						meshBones.bones.push_back(bone);
					}
					skeleton.meshes.emplace_back(std::move(meshBones));
				}

				skeleton.globalInverseTransform = glm::inverse(toglmWeird(scene->mRootNode->mTransformation));

				for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
					const auto aiAnim = scene->mAnimations[i];
					addAnim(aiAnim, skeleton, animList);
				}

				kengine::ModelLoaderComponent::ModelData ret;
				for (const auto & mesh : model.meshes) {
					decltype(ret)::MeshData meshData;
					meshData.vertices = { mesh.vertices.size(), sizeof(ModelEntity::Mesh::Vertex), mesh.vertices.data() };
					meshData.indices = { mesh.indices.size(), sizeof(mesh.indices[0]), mesh.indices.data() };
					meshData.indexType = GL_UNSIGNED_INT;
					ret.meshes.push_back(meshData);
				}

				ret.pitch = model.pitch;
				ret.yaw = model.yaw;
				ret.offsetToCentre = model.offset;
				ret.scale = model.scale;

				return ret;
			};
		}
	}

	AssImpSystem::AssImpSystem(kengine::EntityManager & em) : System(em), _em(em) {
		onLoad("");
	}

	void AssImpSystem::onLoad(const char *) noexcept {
		_em += [this](kengine::Entity & e) {
			e += kengine::makeGBufferShaderComponent<TexturedShader>(_em);
		};

		_em += [this](kengine::Entity & e) {
			e += kengine::makeLightingShaderComponent<AssImpShadowMap>(_em);
			e += kengine::ShadowMapShaderComponent{};
		};

		_em += [&](kengine::Entity & e) {
			e += kengine::makeLightingShaderComponent<AssImpShadowCube>(_em);
			e += kengine::ShadowCubeShaderComponent{};
		};

		_em += [this](kengine::Entity & e) {
			e += ImGuiComponent([this] {
				if (ImGui::Begin("Animations")) {
					for (auto &[obj, model, anim] : _em.getEntities<ModelComponent, AnimationComponent>()) {
						const auto & modelInfo = _em.getEntity(model.modelInfo);
						const auto & animList = modelInfo.get<AnimListComponent>();
						if (animList.allAnims.empty())
							continue;

						const char * tab[64];
						const auto minLength = std::min(animList.allAnims.size(), lengthof(tab));
						for (unsigned int i = 0; i < minLength; ++i)
							tab[i] = animList.allAnims[i].name.data();
						int currentAnim = anim.currentAnim;
						ImGui::Columns(2);
						ImGui::Combo(putils::string<64>("%d", obj.id), &currentAnim, tab, minLength);
						ImGui::NextColumn();
						ImGui::InputFloat(putils::string<64>("Speed##%d", obj.id), &anim.speed);
						ImGui::Columns();
						anim.currentAnim = currentAnim;
					}
				}
				ImGui::End();
			});
		};
	}

	void AssImpSystem::handle(kengine::packets::RegisterEntity p) {
		if (!p.e.has<kengine::GraphicsComponent>())
			return;

		auto & graphics = p.e.get<kengine::GraphicsComponent>();
		const auto & layer = graphics.getLayer("main");
		const auto & file = layer.appearance;

		Assimp::Importer importer;
		if (!importer.IsExtensionSupported(putils::file_extension(file.c_str())))
			return;

		p.e += TexturedModelComponent{};
		p.e += SkeletonComponent{};

		const auto it = AssImp::models.find(file.c_str());
		if (it != AssImp::models.end()) {
			p.e += kengine::ModelComponent{ it->second.id };
			return;
		}

		auto & modelData = AssImp::models[file.c_str()];
		modelData.pitch = layer.pitch;
		modelData.yaw = layer.yaw;
		modelData.offset = layer.boundingBox.topLeft;
		modelData.scale = layer.boundingBox.size;
		_em += [&](kengine::Entity & e) {
			modelData.id = e.id;
			e += kengine::ModelLoaderComponent{
				AssImp::loadFile(file.c_str(), _em),
				[]() { putils::gl::setVertexType<AssImp::ModelEntity::Mesh::Vertex>(); }
			};
		};

		p.e += kengine::ModelComponent{ modelData.id };
	}

	void AssImpSystem::execute() {
		const auto deltaTime = time.getDeltaTime().count();

		for (auto &[e, model, skeleton, anim] : _em.getEntities<ModelComponent, SkeletonComponent, AnimationComponent>()) {
			auto & modelInfo = _em.getEntity(model.modelInfo);
			if (!modelInfo.has<AssImp::AssImpSkeletonComponent>())
				continue;

			auto & assimp = modelInfo.get<AssImp::AssImpSkeletonComponent>();
			const auto & animList = modelInfo.get<AnimListComponent>();

			if (skeleton.meshes.empty())
				skeleton.meshes.resize(assimp.meshes.size());

			for (unsigned int i = 0; i < skeleton.meshes.size(); ++i) {
				auto & input = assimp.meshes[i];
				auto & output = skeleton.meshes[i];

				if (anim.currentAnim >= animList.allAnims.size()) {
					for (auto & bone : input.bones)
						bone.node->mTransformation = AssImp::toAiMat(glm::mat4(1.f));
				}
				else {
					const auto & currentAnim = animList.allAnims[anim.currentAnim];

					for (auto & bone : input.bones)
						updateKeyframeTransform(bone, anim.currentTime * currentAnim.ticksPerSecond, anim.currentAnim);

					anim.currentTime += deltaTime * anim.speed;
					anim.currentTime = fmodf(anim.currentTime, currentAnim.totalTime);
				}

				updateBoneMats(input, output, assimp.globalInverseTransform);
			}
		}
	}
}