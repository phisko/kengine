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
#include "data/InstanceComponent.hpp"
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

#include "helpers/assertHelper.hpp"
#include "AssImpHelper.hpp"

namespace kengine {
	static EntityManager * g_em = nullptr;

#pragma region declarations
	static void execute(float deltaTime);
	static void onEntityCreated(Entity & e);
#pragma endregion
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

#pragma region Helpers
	static Assimp::Importer g_importer;

	static glm::mat4 toglm(const aiMatrix4x4 & mat) {
		return glm::make_mat4(&mat.a1);
	}

	static glm::mat4 toglmWeird(const aiMatrix4x4 & mat) {
		return glm::transpose(toglm(mat));
	}

	static glm::vec3 toglm(const aiVector3D & vec) {
		return { vec.x, vec.y, vec.z };
	}

	static glm::quat toglm(const aiQuaternion & quat) {
		return { quat.w, quat.x, quat.y, quat.z }; 
	}
#pragma endregion Helpers

#pragma region onEntityCreated
#pragma region declarations
	static void loadModel(Entity & e);
	static void setModel(Entity & e);
#pragma endregion
	static void onEntityCreated(Entity & e) {
		if (e.has<ModelComponent>())
			loadModel(e);
		else if (e.has<GraphicsComponent>())
			setModel(e);
	}

	static void setModel(Entity & e) {
		const auto & graphics = e.get<GraphicsComponent>();

		if (!g_importer.IsExtensionSupported(putils::file_extension(graphics.appearance.c_str()).data()))
			return;

		e += AssImpObjectComponent{};
		e += SkeletonComponent{};

		if (e.has<InstanceComponent>() && e.get<InstanceComponent>().model != Entity::INVALID_ID)
			return;

		for (const auto & [model, comp] : g_em->getEntities<ModelComponent>())
			if (comp.file == graphics.appearance) {
				e += InstanceComponent{ model.id };
				return;
			}

		*g_em += [&](Entity & model) {
			model += ModelComponent{ graphics.appearance.c_str() };
			e += InstanceComponent{ model.id };
		};
	}

#pragma region loadModel
#pragma region declarations
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


	static bool loadFile(Entity & e);
	static ModelDataComponent::FreeFunc release(Entity::ID id);
#pragma endregion
	static void loadModel(Entity & e) {
		if (!loadFile(e))
			return;

		ModelDataComponent modelData;

		auto & model = e.get<AssImpModelComponent>();
		for (const auto & mesh : model.meshes) {
			ModelDataComponent::Mesh meshData;
			meshData.vertices = { mesh.vertices.size(), sizeof(AssImpModelComponent::Mesh::Vertex), mesh.vertices.data() };
			meshData.indices = { mesh.indices.size(), sizeof(mesh.indices[0]), mesh.indices.data() };
			meshData.indexType = GL_UNSIGNED_INT;
			modelData.meshes.push_back(meshData);
		}

		modelData.init<AssImpModelComponent::Mesh::Vertex>();
		modelData.free = release(e.id);
		e += std::move(modelData);
	}

#pragma region loadFile
#pragma region declarations
	static void processNode(AssImpModelComponent & modelData, AssImpTexturesModelComponent & textures, const char * directory, const aiNode * node, const aiScene * scene, bool firstLoad);
	static void addNode(std::vector<aiNode *> & allNodes, aiNode * node);
	static aiNode * findNode(const std::vector<aiNode *> & allNodes, const char * name);
	static void addAnim(const char * animFile, aiAnimation * aiAnim, const ModelSkeletonComponent & model, AssImpSkeletonComponent & skeleton, AnimListComponent & animList);
#pragma endregion
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
				kengine_assert_failed(*g_em, putils::string<1024>("Error loading %s: %s", f, model.importer.GetErrorString()));
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
					kengine_assert_failed(*g_em, putils::string<1024>("Error loading anims from %s: %s", f.c_str(), importer.GetErrorString()));
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

#pragma region processNode
#pragma region declarations
	static AssImpModelComponent::Mesh processMesh(const aiMesh * mesh);
	static AssImpTexturesModelComponent::MeshTextures processMeshTextures(const char * directory, const aiMesh * mesh, const aiScene * scene);
#pragma endregion
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
				kengine_assert_with_message(*g_em, found, "Not enough boneWeights available for animation");
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

#pragma region processMeshTextures
#pragma region declarations
	static void loadMaterialTextures(std::vector<Entity::ID> & textures, const char * directory, const aiMaterial * mat, aiTextureType type, const aiScene * scene);
#pragma endregion
	static AssImpTexturesModelComponent::MeshTextures processMeshTextures(const char * directory, const aiMesh * mesh, const aiScene * scene) {
		AssImpTexturesModelComponent::MeshTextures meshTextures;
		if (mesh->mMaterialIndex >= 0) {
			const auto material = scene->mMaterials[mesh->mMaterialIndex];
			loadMaterialTextures(meshTextures.diffuse, directory, material, aiTextureType_DIFFUSE, scene);
			loadMaterialTextures(meshTextures.specular, directory, material, aiTextureType_SPECULAR, scene);

			aiColor3D color{ 0.f, 0.f, 0.f };
			material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
			meshTextures.diffuseColor = { color.r, color.g, color.b };
			material->Get(AI_MATKEY_COLOR_SPECULAR, color);
			meshTextures.specularColor = { color.r, color.g, color.b };
		}
		else
			kengine_assert_failed(*g_em, "Unkown material");
		return meshTextures;
	}

#pragma region loadMaterialTextures
#pragma region declarations
	static Entity::ID loadEmbeddedTexture(const char * path, const aiMaterial * mat, const aiScene * scene);
	static Entity::ID loadFromDisk(const char * directory, const char * file);
#pragma endregion
	static void loadMaterialTextures(std::vector<Entity::ID> & textures, const char * directory, const aiMaterial * mat, aiTextureType type, const aiScene * scene) {
		for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
			aiString path;
			mat->GetTexture(type, i, &path);
			const auto cPath = path.C_Str();

			Entity::ID modelID = Entity::INVALID_ID;
			if (cPath[0] == '*')
				modelID = loadEmbeddedTexture(cPath, mat, scene);
			else
				modelID = loadFromDisk(directory, cPath);

			textures.push_back(modelID);
		}
	}

	static Entity::ID loadEmbeddedTexture(const char * path, const aiMaterial * mat, const aiScene * scene) {

		const auto idx = atoi(path + 1);
		const auto texture = scene->mTextures[idx];

		struct AssimpTextureModelComponent {
			const aiTexture * texture = nullptr;
		};

		for (const auto & [e, model] : g_em->getEntities<AssimpTextureModelComponent>())
			if (model.texture == texture)
				return e.id;

		Entity::ID modelID;

		*g_em += [&](Entity & e) {
			static constexpr auto expectedChannels = 4;
			modelID = e.id;

			e.attach<AssimpTextureModelComponent>().texture = texture;
			auto & comp = e.attach<TextureModelComponent>();
			comp.file = "assimp embedded";
			TextureDataComponent textureLoader; {
				textureLoader.textureID = &comp.texture.get();
				if (texture->mHeight == 0) { // Compressed format
					textureLoader.data = stbi_load_from_memory((unsigned char *)texture->pcData, texture->mWidth, &textureLoader.width, &textureLoader.height, &textureLoader.components, expectedChannels);
					kengine_assert_with_message(*g_em, textureLoader.data != nullptr, "Error loading data from assimp embedded texture");
					textureLoader.free = stbi_image_free;
				}
				else {
					textureLoader.data = texture->pcData;
					textureLoader.width = texture->mWidth;
					textureLoader.height = texture->mHeight;
					textureLoader.components = expectedChannels;
				}
			} e += textureLoader;
		};

		return modelID;
	}

	static Entity::ID loadFromDisk(const char * directory, const char * file) {
		const putils::string<KENGINE_TEXTURE_PATH_MAX_LENGTH> fullPath("%s/%s", directory, file);

		for (const auto & [e, model] : g_em->getEntities<TextureModelComponent>())
			if (model.file == fullPath)
				return e.id;

		Entity::ID modelID;

		*g_em += [&](Entity & e) {
			modelID = e.id;

			auto & comp = e.attach<TextureModelComponent>();
			comp.file = fullPath.c_str();

			TextureDataComponent textureLoader; {
				textureLoader.textureID = &comp.texture.get();

				textureLoader.data = stbi_load(fullPath.c_str(), &textureLoader.width, &textureLoader.height, &textureLoader.components, 0);
				kengine_assert_with_message(*g_em, textureLoader.data != nullptr, putils::string<1024>("Error loading texture file %s", fullPath.c_str()));
				textureLoader.free = stbi_image_free;
			} e += textureLoader;
		};

		return modelID;
	}
#pragma endregion loadMaterialTextures
#pragma endregion processMeshTextures

#pragma endregion processNode

	static void addNode(std::vector<aiNode *> & allNodes, aiNode * node) {
		allNodes.push_back(node);
		for (unsigned int i = 0; i < node->mNumChildren; ++i)
			addNode(allNodes, node->mChildren[i]);
	}

	static aiNode * findNode(const std::vector<aiNode *> & allNodes, const char * name) {
		for (const auto node : allNodes)
			if (strcmp(node->mName.data, name) == 0)
				return node;
		kengine_assert_failed(*g_em, putils::string<1024>("Error finding node %s", name));
		return nullptr;
	}

#pragma region addAnim
#pragma region declarations
	static aiNodeAnim * findNodeAnim(const std::vector<aiNodeAnim *> & allNodes, const char * name);
#pragma endregion
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

	static aiNodeAnim * findNodeAnim(const std::vector<aiNodeAnim *> & allNodes, const char * name) {
		for (const auto node : allNodes)
			if (strcmp(node->mNodeName.data, name) == 0)
				return node;
		return nullptr;
	}
#pragma endregion addAnim
#pragma endregion loadFile

	static ModelDataComponent::FreeFunc release(Entity::ID id) {
		return [id] {
			auto e = g_em->getEntity(id);
			auto & model = e.get<AssImpModelComponent>(); // previous attach hasn't been processed yet, so `get` would assert
			model.importer.FreeScene();
			for (auto & importer : model.animImporters)
				importer.FreeScene();
			e.detach<AssImpModelComponent>();
		};
	}
#pragma endregion loadModel


#pragma endregion onEntityCreated

#pragma region execute
#pragma region declarations
	static void updateBoneMats(const aiNode * node, float time, size_t currentAnim, const AssImpSkeletonComponent & assimp, SkeletonComponent & comp, const glm::mat4 & parentTransform);
#pragma endregion
	static void execute(float deltaTime) {
		for (auto & [e, instance, skeleton, anim] : g_em->getEntities<InstanceComponent, SkeletonComponent, AnimationComponent>())
			g_em->runTask([&] {
				auto & modelEntity = g_em->getEntity(instance.model);
				if (!modelEntity.has<ModelComponent>() || !modelEntity.has<AssImpSkeletonComponent>())
					return;

				const auto & animList = modelEntity.get<AnimListComponent>();

				if (anim.currentAnim >= animList.anims.size())
					return;
				const auto & currentAnim = animList.anims[anim.currentAnim];

				auto & assimp = modelEntity.get<AssImpSkeletonComponent>();

				if (skeleton.meshes.empty())
					skeleton.meshes.resize(assimp.meshes.size());

				updateBoneMats(assimp.rootNode, anim.currentTime * currentAnim.ticksPerSecond, anim.currentAnim, assimp, skeleton, glm::mat4(1.f));

				anim.currentTime += deltaTime * anim.speed;
				anim.currentTime = fmodf(anim.currentTime, currentAnim.totalTime);
			});

		g_em->completeTasks();
	}

#pragma region updateBoneMats
#pragma region declarations
	static glm::vec3 calculateInterpolatedPosition(const AssImpSkeletonComponent::Mesh::Bone & bone, float time, size_t currentAnim);
	static glm::quat calculateInterpolatedRotation(const AssImpSkeletonComponent::Mesh::Bone & bone, float time, size_t currentAnim);
	static glm::vec3 calculateInterpolatedScale(const AssImpSkeletonComponent::Mesh::Bone & bone, float time, size_t currentAnim);
#pragma endregion
	static void updateBoneMats(const aiNode * node, float time, size_t currentAnim, const AssImpSkeletonComponent & assimp, SkeletonComponent & comp, const glm::mat4 & parentTransform) {
		bool firstCalc = true;
		glm::mat4 totalTransform = parentTransform * toglmWeird(node->mTransformation);

		for (unsigned int i = 0; i < assimp.meshes.size(); ++i) {
			const auto & input = assimp.meshes[i];
			auto & output = comp.meshes[i];

			kengine_assert_with_message(*g_em, input.bones.size() < lengthof(output.boneMatsBoneSpace), "Not enough bones in SkeletonComponent. You need to increase KENGIEN_SKELETON_MAX_BONES");

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

#pragma region Template helper
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
#pragma endregion Template helper

	static glm::vec3 calculateInterpolatedPosition(const AssImpSkeletonComponent::Mesh::Bone & bone, float time, size_t currentAnim) {
		return calculateInterpolatedValue(bone.animNodes[currentAnim]->mPositionKeys, bone.animNodes[currentAnim]->mNumPositionKeys, time, [](const glm::vec3 & v1, const glm::vec3 & v2, float f) { return glm::mix(v1, v2, f); });
	}

	static glm::quat calculateInterpolatedRotation(const AssImpSkeletonComponent::Mesh::Bone & bone, float time, size_t currentAnim) {
		return calculateInterpolatedValue(bone.animNodes[currentAnim]->mRotationKeys, bone.animNodes[currentAnim]->mNumRotationKeys, time, glm::slerp<float, glm::defaultp>);
	}

	static glm::vec3 calculateInterpolatedScale(const AssImpSkeletonComponent::Mesh::Bone & bone, float time, size_t currentAnim) {
		return calculateInterpolatedValue(bone.animNodes[currentAnim]->mScalingKeys, bone.animNodes[currentAnim]->mNumScalingKeys, time, [](const glm::vec3 & v1, const glm::vec3 & v2, float f) { return glm::mix(v1, v2, f); });
	}
#pragma endregion updateBoneMats
#pragma endregion execute
}