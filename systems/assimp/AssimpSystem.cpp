#include "AssimpSystem.hpp"
#include "EntityManager.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "stb_image.h"

#include "file_extension.hpp"
#include "imgui.h"

#include "AssImpShader.hpp"
#include "AssImpShadowMap.hpp"
#include "AssImpShadowCube.hpp"

#include "data/GraphicsComponent.hpp"
#include "data/ModelDataComponent.hpp"
#include "data/TextureDataComponent.hpp"
#include "data/TextureModelComponent.hpp"
#include "data/ModelComponent.hpp"

#include "data/AnimationComponent.hpp"
#include "data/SkeletonComponent.hpp"
#include "data/ModelSkeletonComponent.hpp"

#include "data/ShaderComponent.hpp"
#include "data/ImGuiComponent.hpp"

#include "functions/Execute.hpp"
#include "functions/OnEntityCreated.hpp"

#include "AssImpHelper.hpp"

namespace kengine {
	static EntityManager * g_em = nullptr;

	// declarations
	static void execute(float deltaTime);
	static void onEntityCreated(Entity & e);
	//
	EntityCreator * AssImpSystem(EntityManager & em) {
		g_em = &em;

		em += [&](Entity & e) {
			e += makeGBufferShaderComponent<AssImpShader>(em);
		};

		em += [&](Entity & e) {
			e += makeLightingShaderComponent<AssImpShadowMap>(em);
			e += ShadowMapShaderComponent{};
		};

		em += [&](Entity & e) {
			e += makeLightingShaderComponent<AssImpShadowCube>(em);
			e += ShadowCubeShaderComponent{};
		};

		return [](Entity & e) {
			e += functions::Execute{ execute };
			e += functions::OnEntityCreated{ onEntityCreated };
		};
	}

	// declarations
	static void loadModel(Entity & e);
	static void setModel(Entity & e);
	//
	static void onEntityCreated(Entity & e) {
		if (e.has<ModelComponent>())
			loadModel(e);
		else if (e.has<GraphicsComponent>())
			setModel(e);
	}

	static Assimp::Importer g_importer;

	namespace AssImp {

		struct AssImpModelComponent {
			struct Mesh {
				struct Vertex {
					float position[3];
					float normal[3];
					float texCoords[2];
					float boneWeights[KENGINE_ASSIMP_BONE_INFO_PER_VERTEX] = { 0.f };
					unsigned int boneIDs[KENGINE_ASSIMP_BONE_INFO_PER_VERTEX] = { 0 };

					putils_reflection_attributes(
						putils_reflection_attribute(&Vertex::position),
						putils_reflection_attribute(&Vertex::normal),
						putils_reflection_attribute(&Vertex::texCoords),

						putils_reflection_attribute(&Vertex::boneWeights),
						putils_reflection_attribute(&Vertex::boneIDs)
					);
				};

				std::vector<Vertex> vertices;
				std::vector<unsigned int> indices;
			};

			Assimp::Importer importer;
			std::vector<Assimp::Importer> animImporters;
			std::vector<Mesh> meshes;
		};

		struct AssImpSkeletonComponent {
			struct Mesh {
				struct Bone {
					aiNode * node = nullptr;
					std::vector<const aiNodeAnim *> animNodes;
					glm::mat4 offset;
				};
				std::vector<Bone> bones;
			};

			aiNode * rootNode;
			std::vector<Mesh> meshes;
			glm::mat4 globalInverseTransform;
		};

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

		static glm::vec3 calculateInterpolatedPosition(const AssImpSkeletonComponent::Mesh::Bone & bone, float time, size_t currentAnim) {
			return calculateInterpolatedValue(bone.animNodes[currentAnim]->mPositionKeys, bone.animNodes[currentAnim]->mNumPositionKeys, time, [](const glm::vec3 & v1, const glm::vec3 & v2, float f) { return glm::mix(v1, v2, f); });
		}

		static glm::quat calculateInterpolatedRotation(const AssImpSkeletonComponent::Mesh::Bone & bone, float time, size_t currentAnim) {
			return calculateInterpolatedValue(bone.animNodes[currentAnim]->mRotationKeys, bone.animNodes[currentAnim]->mNumRotationKeys, time, glm::slerp<float, glm::defaultp>);
		}

		static glm::vec3 calculateInterpolatedScale(const AssImpSkeletonComponent::Mesh::Bone & bone, float time, size_t currentAnim) {
			return calculateInterpolatedValue(bone.animNodes[currentAnim]->mScalingKeys, bone.animNodes[currentAnim]->mNumScalingKeys, time, [](const glm::vec3 & v1, const glm::vec3 & v2, float f) { return glm::mix(v1, v2, f); });
		}

		static void updateBoneMats(const aiNode * node, float time, size_t currentAnim, const AssImpSkeletonComponent & assimp, SkeletonComponent & comp, const glm::mat4 & parentTransform) {
			bool firstCalc = true;
			glm::mat4 totalTransform = parentTransform * toglmWeird(node->mTransformation);

			for (unsigned int i = 0; i < assimp.meshes.size(); ++i) {
				const auto & input = assimp.meshes[i];
				auto & output = comp.meshes[i];

				assert(input.bones.size() < lengthof(output.boneMatsBoneSpace)); // Need to increase KENGINE_SKELETON_MAX_BONES

				size_t boneIndex = 0;
				for (const auto & bone : input.bones) {
					if (bone.node == node)
						break;
					++boneIndex;
				}

				if (boneIndex != input.bones.size()) {
					const auto & modelBone = input.bones[boneIndex];

					if (firstCalc) {
						glm::mat4 mat(1.f);
						if (modelBone.animNodes[currentAnim] != nullptr) {
							const auto pos = calculateInterpolatedPosition(modelBone, time, currentAnim);
							const auto rot = calculateInterpolatedRotation(modelBone, time, currentAnim);
							const auto scale = calculateInterpolatedScale(modelBone, time, currentAnim);

							mat = glm::translate(mat, pos);
							mat *= glm::mat4_cast(rot);
							mat = glm::scale(mat, scale);
						}
						totalTransform = parentTransform * mat;

						firstCalc = false;
					}

					output.boneMatsMeshSpace[boneIndex] = totalTransform;
					output.boneMatsBoneSpace[boneIndex] = totalTransform * modelBone.offset;
				}
			}

			for (size_t i = 0; i < node->mNumChildren; ++i)
				updateBoneMats(node->mChildren[i], time, currentAnim, assimp, comp, totalTransform);
		}

		static void loadMaterialTextures(std::vector<Entity::ID> & textures, const char * directory, const aiMaterial * mat, aiTextureType type) {
			for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
				aiString path;
				mat->GetTexture(type, i, &path);

				const putils::string<KENGINE_TEXTURE_PATH_MAX_LENGTH> fullPath("%s/%s", directory, path.C_Str());

				Entity::ID modelID = Entity::INVALID_ID;
				for (const auto &[e, model] : g_em->getEntities<TextureModelComponent>())
					if (model.file == fullPath) {
						modelID = e.id;
						break;
					}

				if (modelID == Entity::INVALID_ID) {
					*g_em += [&](Entity & e) {
						modelID = e.id;

						auto & comp = e.attach<TextureModelComponent>();
						comp.file = fullPath.c_str();

						TextureDataComponent textureLoader; {
							textureLoader.textureID = &comp.texture.get();

							textureLoader.data = stbi_load(fullPath.c_str(), &textureLoader.width, &textureLoader.height, &textureLoader.components, 0);
							assert(textureLoader.data != nullptr);
							textureLoader.free = stbi_image_free;
						} e += textureLoader;
					};
				}

				textures.push_back(modelID);
			}
		}

		static AssImpModelComponent::Mesh processMesh(const aiMesh * mesh) {
			AssImpModelComponent::Mesh ret;

			for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
				AssImpModelComponent::Mesh::Vertex vertex;

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
					bool found = false;
					for (unsigned int k = 0; k < KENGINE_ASSIMP_BONE_INFO_PER_VERTEX; ++k)
						if (vertex.boneWeights[k] == 0.f) {
							found = true;
							vertex.boneWeights[k] = weight.mWeight;
							vertex.boneIDs[k] = i;
							break;
						}
					assert(found); // too many bones have info for a single vertex
					if (!found) {
						float smallestWeight = FLT_MAX;
						unsigned int smallestIndex = 0;
						for (unsigned int k = 0; k < KENGINE_ASSIMP_BONE_INFO_PER_VERTEX; ++k)
							if (vertex.boneWeights[k] < smallestWeight) {
								smallestWeight = vertex.boneWeights[k];
								smallestIndex = k;
							}
						if (weight.mWeight > smallestWeight)
							vertex.boneWeights[smallestIndex] = weight.mWeight;
					}
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
			return ret;
		}

		static AssImpTexturesModelComponent::MeshTextures processMeshTextures(const char * directory, const aiMesh * mesh, const aiScene * scene) {
			AssImpTexturesModelComponent::MeshTextures meshTextures;
			if (mesh->mMaterialIndex >= 0) {
				const auto material = scene->mMaterials[mesh->mMaterialIndex];
				loadMaterialTextures(meshTextures.diffuse, directory, material, aiTextureType_DIFFUSE);
				loadMaterialTextures(meshTextures.specular, directory, material, aiTextureType_SPECULAR);

				aiColor3D color{ 0.f, 0.f, 0.f };
				material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
				meshTextures.diffuseColor = { color.r, color.g, color.b };
				material->Get(AI_MATKEY_COLOR_SPECULAR, color);
				meshTextures.specularColor = { color.r, color.g, color.b };
			}
			else
				assert(false);
			return meshTextures;
		}

		static void processNode(AssImpModelComponent & modelData, AssImpTexturesModelComponent & textures, const char * directory, const aiNode * node, const aiScene * scene, bool firstLoad) {
			for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
				const aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
				if (firstLoad)
					modelData.meshes.push_back(processMesh(mesh));
				textures.meshes.push_back(processMeshTextures(directory, mesh, scene));
			}

			for (unsigned int i = 0; i < node->mNumChildren; ++i)
				processNode(modelData, textures, directory, node->mChildren[i], scene, firstLoad);
		}

		static void addNode(std::vector<aiNode *> & allNodes, aiNode * node) {
			allNodes.push_back(node);
			for (unsigned int i = 0; i < node->mNumChildren; ++i)
				addNode(allNodes, node->mChildren[i]);
		}

		static const AssImpSkeletonComponent::Mesh::Bone * findBone(const ModelSkeletonComponent::Mesh & model, const AssImpSkeletonComponent::Mesh & skeleton, const char * name) {
			for (unsigned int i = 0; i < skeleton.bones.size(); ++i)
				if (model.boneNames[i] == name)
					return &skeleton.bones[i];
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

		static void addAnim(const char * animFile, aiAnimation * aiAnim, const ModelSkeletonComponent & model, AssImpSkeletonComponent & skeleton, AnimListComponent & animList) {
			AnimListComponent::Anim anim;
			anim.name = animFile;
			anim.name += "/";
			anim.name += aiAnim->mName.data;
			anim.ticksPerSecond = (float)(aiAnim->mTicksPerSecond != 0 ? aiAnim->mTicksPerSecond : 25.0);
			anim.totalTime = (float)aiAnim->mDuration / anim.ticksPerSecond;
			animList.anims.push_back(std::move(anim));

			std::vector<aiNodeAnim *> allNodeAnims;
			for (unsigned int i = 0; i < aiAnim->mNumChannels; ++i)
				allNodeAnims.push_back(aiAnim->mChannels[i]);

			for (unsigned int i = 0; i < skeleton.meshes.size(); ++i) {
				auto & mesh = skeleton.meshes[i];
				for (unsigned int j = 0; j < mesh.bones.size(); ++j) {
					auto & bone = mesh.bones[j];
					const auto & boneName = model.meshes[i].boneNames[j];
					bone.animNodes.push_back(findNodeAnim(allNodeAnims, boneName.c_str()));
				}
			}
		}

		static auto release(Entity::ID id) {
			return [id] {
#if 0 // Disabled because scene is still used for animation at this point
				auto & e = g_em->getEntity(id);

				auto & model = e.attach<AssImpModelComponent>(); // previous attach hasn't been processed yet, so `get` would assert
				model.importer.FreeScene();
				for (auto & importer : model.animImporters)
					importer.FreeScene();
				e.detach<AssImpModelComponent>();
#endif
			};
		}

		static bool loadFile(Entity & e) {
			const auto & f = e.get<ModelComponent>().file.c_str();
			if (!g_importer.IsExtensionSupported(putils::file_extension(f).data()))
				return false;

#ifndef KENGINE_NDEBUG
			std::cout << putils::termcolor::green << "[AssImp] Loading " << putils::termcolor::cyan << f << putils::termcolor::green << "..." << putils::termcolor::reset;
#endif

			auto & model = e.attach<AssImpModelComponent>();

			bool firstLoad = false;
			if (model.importer.GetScene() == nullptr) {
				const auto scene = model.importer.ReadFile(f, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals /*| aiProcess_OptimizeMeshes*/ | aiProcess_JoinIdenticalVertices);
				if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
					std::cerr << putils::termcolor::red << model.importer.GetErrorString() << '\n' << putils::termcolor::reset;
					assert(false);
				}
				firstLoad = true;
			}
			const auto scene = model.importer.GetScene();

			const auto dir = putils::get_directory(f);
			auto & textures = e.attach<AssImpTexturesModelComponent>();
			processNode(model, textures, putils::string<64>(dir), scene->mRootNode, scene, firstLoad);

			std::vector<aiNode *> allNodes;
			addNode(allNodes, scene->mRootNode);

			auto & skeleton = e.attach<AssImpSkeletonComponent>();
			skeleton.rootNode = scene->mRootNode;

			auto & skeletonNames = e.attach<ModelSkeletonComponent>();
			for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
				const auto mesh = scene->mMeshes[i];

				ModelSkeletonComponent::Mesh meshNames;
				AssImpSkeletonComponent::Mesh meshBones;
				for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
					const auto aiBone = mesh->mBones[i];
					const auto name = aiBone->mName.data;

					AssImpSkeletonComponent::Mesh::Bone bone;
					bone.node = findNode(allNodes, name);
					bone.offset = toglmWeird(aiBone->mOffsetMatrix);
					meshBones.bones.push_back(bone);

					meshNames.boneNames.push_back(name);
				}
				skeleton.meshes.emplace_back(std::move(meshBones));
				skeletonNames.meshes.emplace_back(std::move(meshNames));
			}

			skeleton.globalInverseTransform = glm::inverse(toglmWeird(scene->mRootNode->mTransformation));

			auto & animList = e.attach<AnimListComponent>();
			for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
				addAnim(f, scene->mAnimations[i], skeletonNames, skeleton, animList);

			if (e.has<AnimFilesComponent>()) {
				const auto & animFiles = e.get<AnimFilesComponent>();
				model.animImporters.resize(animFiles.files.size());

				size_t i = 0;
				for (const auto & f : animFiles.files) {
					auto & importer = model.animImporters[i];

					const auto scene = importer.ReadFile(f.c_str(), aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals /*| aiProcess_OptimizeMeshes*/ | aiProcess_JoinIdenticalVertices);
					if (scene == nullptr || scene->mRootNode == nullptr) {
						std::cerr << '\n' << putils::termcolor::red << importer.GetErrorString() << '\n' << putils::termcolor::reset;
						assert(false);
					}

					for (unsigned int i = 0; i < scene->mNumAnimations; ++i)
						addAnim(f.c_str(), scene->mAnimations[i], skeletonNames, skeleton, animList);

					++i;
				}
			}

#ifndef KENGINE_NDEBUG
			std::cout << putils::termcolor::green << "Done\n" << putils::termcolor::reset;
#endif
			return true;
		}
	}

	static void execute(float deltaTime) {
		for (auto & [e, graphics, skeleton, anim] : g_em->getEntities<GraphicsComponent, SkeletonComponent, AnimationComponent>())
			g_em->runTask([&] {
				if (graphics.model == Entity::INVALID_ID)
					return;

				auto & modelEntity = g_em->getEntity(graphics.model);
				if (!modelEntity.has<ModelComponent>() || !modelEntity.has<AssImp::AssImpSkeletonComponent>())
					return;

				const auto & animList = modelEntity.get<AnimListComponent>();

				if (anim.currentAnim >= animList.anims.size())
					return;
				const auto & currentAnim = animList.anims[anim.currentAnim];

				auto & assimp = modelEntity.get<AssImp::AssImpSkeletonComponent>();

				if (skeleton.meshes.empty())
					skeleton.meshes.resize(assimp.meshes.size());

				AssImp::updateBoneMats(assimp.rootNode, anim.currentTime * currentAnim.ticksPerSecond, anim.currentAnim, assimp, skeleton, glm::mat4(1.f));

				anim.currentTime += deltaTime * anim.speed;
				anim.currentTime = fmodf(anim.currentTime, currentAnim.totalTime);
			});

		g_em->completeTasks();
	}

	static void loadModel(Entity & e) {
		if (!AssImp::loadFile(e))
			return;

		ModelDataComponent modelData;

		auto & model = e.get<AssImp::AssImpModelComponent>();
		for (const auto & mesh : model.meshes) {
			ModelDataComponent::Mesh meshData;
			meshData.vertices = { mesh.vertices.size(), sizeof(AssImp::AssImpModelComponent::Mesh::Vertex), mesh.vertices.data() };
			meshData.indices = { mesh.indices.size(), sizeof(mesh.indices[0]), mesh.indices.data() };
			meshData.indexType = GL_UNSIGNED_INT;
			modelData.meshes.push_back(meshData);
		}
		
		modelData.init<AssImp::AssImpModelComponent::Mesh::Vertex>();
		modelData.free = AssImp::release(e.id);
		e += std::move(modelData);
	}

	static void setModel(Entity & e) {
		auto & graphics = e.get<GraphicsComponent>();

		if (!g_importer.IsExtensionSupported(putils::file_extension(graphics.appearance.c_str()).data()))
			return;

		e += AssImpObjectComponent{};
		e += SkeletonComponent{};

		graphics.model = Entity::INVALID_ID;
		for (const auto &[e, model] : g_em->getEntities<ModelComponent>())
			if (model.file == graphics.appearance) {
				graphics.model = e.id;
				return;
			}

		*g_em += [&](Entity & e) {
			e += ModelComponent{ graphics.appearance.c_str() };
			graphics.model = e.id;
		};
	}
}