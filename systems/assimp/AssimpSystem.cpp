#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stb_image.h>

#include "AssimpSystem.hpp"
#include "kengine.hpp"

#include "AssImpShader.hpp"
#include "AssImpShadowMap.hpp"
#include "AssImpShadowCube.hpp"

#include "data/AdjustableComponent.hpp"
#include "data/AnimationComponent.hpp"
#include "data/AnimationFilesComponent.hpp"
#include "data/GraphicsComponent.hpp"
#include "data/InstanceComponent.hpp"
#include "data/ModelAnimationComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/ModelDataComponent.hpp"
#include "data/ModelSkeletonComponent.hpp"
#include "data/ShaderComponent.hpp"
#include "data/SkeletonComponent.hpp"
#include "data/TextureDataComponent.hpp"

#include "functions/Execute.hpp"
#include "functions/OnEntityCreated.hpp"

#include "helpers/assertHelper.hpp"
#include "helpers/matrixHelper.hpp"
#include "helpers/resourceHelper.hpp"
#include "helpers/instanceHelper.hpp"
#include "helpers/logHelper.hpp"
#include "AssImpHelper.hpp"

#include "opengl/Program.hpp"

#include "file_extension.hpp"
#include "on_scope_exit.hpp"
#include "imgui.h"

namespace kengine::assimp {
	struct AssImpAnimFileComponent { // Entity that represents an animation file
		std::string fileName;
		std::unique_ptr<Assimp::Importer> importer;
	};

	struct AssImpModelAnimComponent {
		struct AnimEntity {
			EntityID id;
			size_t nbAnims;
		};
		std::vector<AnimEntity> animEntities;
	};

	struct AssImpModelComponent {
		struct Mesh {
			struct Vertex {
				float position[3];
				float normal[3];
				float texCoords[2];
				float boneWeights[KENGINE_ASSIMP_BONE_INFO_PER_VERTEX] = { 0.f };
				unsigned int boneIDs[KENGINE_ASSIMP_BONE_INFO_PER_VERTEX] = { 0 };
			};

			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;
		};

		std::unique_ptr<Assimp::Importer> importer;
		std::vector<Mesh> meshes;
	};

	struct AssImpModelSkeletonComponent {
		struct Mesh {
			struct Bone {
				aiNode * node = nullptr;
				glm::mat4 offset;
			};
			std::vector<Bone> bones;
		};

		aiNode * rootNode;
		std::vector<Mesh> meshes;
		glm::mat4 globalInverseTransform;
	};
}

#define refltype kengine::assimp::AssImpModelComponent::Mesh::Vertex
putils_reflection_info {
	putils_reflection_attributes(
		putils_reflection_attribute(position),
		putils_reflection_attribute(normal),
		putils_reflection_attribute(texCoords),

		putils_reflection_attribute(boneWeights),
		putils_reflection_attribute(boneIDs)
	);
};
#undef refltype

namespace kengine::assimp {
	struct {
		bool editorMode = false;
	} adjustables;

	struct impl {
		static void init(Entity & e) noexcept {
			kengine_log(Log, "Init", "AssimpSystem");

			e += functions::Execute{ execute };
			e += functions::OnEntityCreated{ onEntityCreated };

			e += AdjustableComponent{
				"Animation", {
					{ "Editor mode (reload files each frame)", &adjustables.editorMode }
				}
			};

			entities += [&](Entity & e) noexcept {
				e += SystemSpecificShaderComponent<putils::gl::Program>{ std::make_unique<AssImpShader>() };
				e += GBufferShaderComponent{};
			};

			entities += [&](Entity & e) noexcept {
				e += SystemSpecificShaderComponent<putils::gl::Program>{ std::make_unique<AssImpShadowMap>() };
				e += ShadowMapShaderComponent{};
			};

			entities += [&](Entity & e) {
				e += SystemSpecificShaderComponent<putils::gl::Program>{ std::make_unique<AssImpShadowCube>() };
				e += ShadowCubeShaderComponent{};
			};
		}

		static void onEntityCreated(Entity & e) noexcept {
			if (e.has<ModelComponent>())
				loadModel(e);
			else if (e.has<GraphicsComponent>())
				initObject(e);
		}

		static void initObject(Entity & e) noexcept {
			const auto & graphics = e.get<GraphicsComponent>();
			const char * path = graphics.appearance;

			const auto instance = e.tryGet<InstanceComponent>();
			if (instance && instance->model != INVALID_ID) {
				const auto &model = instanceHelper::getModel<ModelComponent>(*instance);
				path = model.file;
			}

			if (!helpers::importer.IsExtensionSupported(putils::file_extension(path).data()))
				return;

			kengine_logf(Verbose, "AssimpSystem", "Marking %zu as an AssImp object", e.id);
			e += AssImpObjectComponent{};
			e += SkeletonComponent{};
		}

#pragma region loadModel
		static void loadModel(Entity & e) noexcept {
			if (!loadFile(e))
				return;

			ModelDataComponent modelData;

			auto & model = e.get<AssImpModelComponent>();
			for (const auto & mesh : model.meshes) {
				ModelDataComponent::Mesh meshData;
				meshData.vertices = { mesh.vertices.size(), sizeof(AssImpModelComponent::Mesh::Vertex), mesh.vertices.data() };
				meshData.indices = { mesh.indices.size(), sizeof(mesh.indices[0]), mesh.indices.data() };
				meshData.indexType = putils::meta::type<unsigned int>::index;
				modelData.meshes.push_back(meshData);
			}

			modelData.init<AssImpModelComponent::Mesh::Vertex>();
			modelData.free = release(e.id);
			e += std::move(modelData);
		}

		static bool loadFile(Entity & e) noexcept {
			const auto & f = e.get<ModelComponent>().file.c_str();
			if (!helpers::importer.IsExtensionSupported(putils::file_extension(f).data()))
				return false;

			kengine_logf(Log, "AssimpSystem", "Loading %s for %zu", f, e.id);

			auto & model = e.attach<AssImpModelComponent>();
			model.importer = std::make_unique<Assimp::Importer>();

			if (model.importer->GetScene() == nullptr) {
				const auto scene = model.importer->ReadFile(f, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals /*| aiProcess_OptimizeMeshes*/ | aiProcess_JoinIdenticalVertices);
				if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr) {
					std::cerr << putils::termcolor::red << model.importer->GetErrorString() << '\n' << putils::termcolor::reset;
					kengine_assert_failed(putils::string<1024>("Error loading %s: %s", f, model.importer->GetErrorString()).c_str());
					return false;
				}
			}
			const auto scene = model.importer->GetScene();

			loadMeshes(e, model, f, scene);
			loadSkeleton(e, scene);
			loadAnims(e, f, scene);

			return true;
		}

		static void loadMeshes(Entity & e, AssImpModelComponent & model, const char * file, const aiScene * scene) noexcept {
			const auto dir = putils::get_directory(file);
			auto & textures = e.attach<AssImpTexturesModelComponent>();
			processNode(model, textures, putils::string<1024>(dir), scene->mRootNode, scene);
		}

		static void processNode(AssImpModelComponent & modelData, AssImpTexturesModelComponent & textures, const char * directory, const aiNode * node, const aiScene * scene) noexcept {
			for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
				const aiMesh * mesh = scene->mMeshes[node->mMeshes[i]];
				kengine_logf(Log, "AssimpSystem/loadFile/loadMeshes", "Loading mesh %s", mesh->mName.C_Str());
				modelData.meshes.push_back(processMesh(mesh));
				textures.meshes.push_back(processMeshTextures(directory, mesh, scene));
			}

			for (unsigned int i = 0; i < node->mNumChildren; ++i)
				processNode(modelData, textures, directory, node->mChildren[i], scene);
		}

		static AssImpModelComponent::Mesh processMesh(const aiMesh * mesh) noexcept {
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
					kengine_assert_with_message(found, "Not enough boneWeights available for animation");
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

		static AssImpTexturesModelComponent::MeshTextures processMeshTextures(const char * directory, const aiMesh * mesh, const aiScene * scene) noexcept {
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
				kengine_assert_failed("Unkown material");
			return meshTextures;
		}

		static void loadMaterialTextures(std::vector<EntityID> & textures, const char * directory, const aiMaterial * mat, aiTextureType type, const aiScene * scene) noexcept {
			for (unsigned int i = 0; i < mat->GetTextureCount(type); ++i) {
				aiString path;
				mat->GetTexture(type, i, &path);
				const auto cPath = path.C_Str();

				EntityID modelID = INVALID_ID;
				if (const auto texture = scene->GetEmbeddedTexture(cPath))
					modelID = resourceHelper::createTextureModel(texture->pcData, texture->mWidth, texture->mHeight);
				else
					modelID = resourceHelper::createTextureModel(putils::string<KENGINE_TEXTURE_PATH_MAX_LENGTH>("%s/%s", directory, cPath));

				textures.push_back(modelID);
			}
		}

		static void loadSkeleton(Entity & e, const aiScene * scene) noexcept {
			kengine_log(Log, "AssimpSystem/loadFile", "Loading skeleton");

			std::vector<aiNode *> allNodes;
			addNode(allNodes, scene->mRootNode);

			auto & skeleton = e.attach<AssImpModelSkeletonComponent>();
			skeleton.rootNode = scene->mRootNode;

			auto & skeletonNames = e.attach<ModelSkeletonComponent>();
			for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
				const auto mesh = scene->mMeshes[i];

				ModelSkeletonComponent::Mesh meshNames;
				AssImpModelSkeletonComponent::Mesh meshBones;
				for (unsigned int i = 0; i < mesh->mNumBones; ++i) {
					const auto aiBone = mesh->mBones[i];
					const auto name = aiBone->mName.data;

					kengine_logf(Log, "AssimpSystem/loadFile/loadSkeleton", "Adding bone %s", name);

					AssImpModelSkeletonComponent::Mesh::Bone bone;
					bone.node = findNode(allNodes, name);
					bone.offset = helpers::toglmWeird(aiBone->mOffsetMatrix);
					meshBones.bones.push_back(bone);

					meshNames.boneNames.push_back(name);
				}
				skeleton.meshes.emplace_back(std::move(meshBones));
				skeletonNames.meshes.emplace_back(std::move(meshNames));
			}

			skeleton.globalInverseTransform = glm::inverse(helpers::toglmWeird(scene->mRootNode->mTransformation));
		}

		static void addNode(std::vector<aiNode *> & allNodes, aiNode * node) noexcept {
			allNodes.push_back(node);
			for (unsigned int i = 0; i < node->mNumChildren; ++i)
				addNode(allNodes, node->mChildren[i]);
		}

		static aiNode * findNode(const std::vector<aiNode *> & allNodes, const char * name) noexcept {
			for (const auto node : allNodes)
				if (strcmp(node->mName.data, name) == 0)
					return node;
			kengine_assert_failed(putils::string<1024>("Error finding node %s", name).c_str());
			return nullptr;
		}

		static void loadAnims(Entity & e, const char * file, const aiScene * scene) noexcept {
			kengine_log(Log, "AssimpSystem/loadFile", "Loading animations");

			auto & anims = e.attach<ModelAnimationComponent>();

			initExtractedMotionGetters(e, anims);

			anims.animations.clear();
			addAnims(file, scene, anims);

			const auto animFiles = e.tryGet<AnimationFilesComponent>();
			if (!animFiles)
				return;

			auto & assimpAnimFiles = e.attach<AssImpModelAnimComponent>();
			assimpAnimFiles.animEntities.clear();

			for (const auto & f : animFiles->files) {
				const auto animEntity = loadAnimFile(f.c_str());
				assimpAnimFiles.animEntities.push_back(animEntity);

				const auto & assimpAnim = entities[animEntity.id].get<AssImpAnimFileComponent>();
				addAnims(f.c_str(), assimpAnim.importer->GetScene(), anims);
			}
		}

		static AssImpModelAnimComponent::AnimEntity loadAnimFile(const char * file) noexcept {
			AssImpModelAnimComponent::AnimEntity ret;

			for (const auto & [e, anim] : entities.with<AssImpAnimFileComponent>())
				if (anim.fileName == file) {
					ret.id = e.id;
					ret.nbAnims = anim.importer->GetScene()->mNumAnimations;
					return ret;
				}

			entities += [&](Entity & e) noexcept {
				ret.id = e.id;

				auto & comp = e.attach<AssImpAnimFileComponent>();
				comp.fileName = file;
				comp.importer = std::make_unique<Assimp::Importer>();

				const auto scene = comp.importer->ReadFile(file, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals /*| aiProcess_OptimizeMeshes*/ | aiProcess_JoinIdenticalVertices);
				if (scene == nullptr || scene->mRootNode == nullptr)
					kengine_assert_failed(putils::string<1024>("Error loading anims from %s: %s", file, comp.importer->GetErrorString()).c_str());
				else
					ret.nbAnims = scene->mNumAnimations;
			};

			return ret;
		}

		static void addAnims(const char * animFile, const aiScene * scene, ModelAnimationComponent & anims) noexcept {
			kengine_logf(Log, "AssimpSystem/loadFile/loadAnims", "Loading animations from %s", animFile);
			for (unsigned int i = 0; i < scene->mNumAnimations; ++i) {
				const auto aiAnim = scene->mAnimations[i];
				kengine_logf(Log, "AssimpSystem/loadFile/loadAnims", "Found animation '%s'", aiAnim->mName.C_Str());
				ModelAnimationComponent::Anim anim;
				anim.name = animFile;
				anim.name += '/';
				anim.name += aiAnim->mName.C_Str();
				anim.ticksPerSecond = (float)(aiAnim->mTicksPerSecond != 0 ? aiAnim->mTicksPerSecond : 25.0);
				anim.totalTime = (float)aiAnim->mDuration / anim.ticksPerSecond;
				anims.animations.emplace_back(std::move(anim));
			}
		}

		static void initExtractedMotionGetters(const Entity & e, ModelAnimationComponent & anims) noexcept {
			struct MovementExtractorParams {
				glm::mat4 toWorldSpace;
				const aiNodeAnim * nodeAnim;
			};

			const auto getExtractorParams = [modelID = e.id](const Entity & e, size_t anim) noexcept {
				const auto modelEntity = entities[modelID];

				auto noTranslateTransform = e.get<TransformComponent>();
				noTranslateTransform.boundingBox.position = putils::Point3f{};
				const auto modelMatrix = matrixHelper::getModelMatrix(noTranslateTransform, modelEntity.tryGet<TransformComponent>());

				glm::mat4 parentTransform(1.f);
				const auto nodeAnim = getRootNodeAnim(modelEntity, getAnimation(modelEntity, anim), parentTransform);

				return MovementExtractorParams{ parentTransform * modelMatrix, nodeAnim };
			};

			anims.getAnimationMovementUntilTime = [getExtractorParams](const Entity & e, size_t anim, float time) noexcept {
				const auto params = getExtractorParams(e, anim);
				const auto pos = calculateInterpolatedPosition(params.nodeAnim, time);
				return matrixHelper::convertToReferencial(glm::value_ptr(pos), params.toWorldSpace);
			};

			anims.getAnimationRotationUntilTime = [getExtractorParams](const Entity & e, size_t anim, float time) noexcept {
				const auto params = getExtractorParams(e, anim);
				const auto rot = calculateInterpolatedRotation(params.nodeAnim, time);
				return matrixHelper::getRotation(glm::mat4_cast(rot));
			};

			anims.getAnimationScalingUntilTime = [getExtractorParams](const Entity & e, size_t anim, float time) noexcept {
				const auto params = getExtractorParams(e, anim);
				const auto scale = calculateInterpolatedScale(params.nodeAnim, time);
				return matrixHelper::convertToReferencial(glm::value_ptr(scale), params.toWorldSpace);
			};
		}

		static aiAnimation * getAnimation(const Entity & modelEntity, size_t anim) noexcept {
			const auto & assimpModel = modelEntity.get<AssImpModelComponent>();
			const auto scene = assimpModel.importer->GetScene();
			const auto nbAnimsInModel = scene->mNumAnimations;
			if (anim < nbAnimsInModel)
				return scene->mAnimations[anim];

			// Find anim in AnimFilesComponent
			const auto & assimpAnimFiles = modelEntity.get<AssImpModelAnimComponent>();
			size_t i = nbAnimsInModel;
			for (const auto & e : assimpAnimFiles.animEntities) {
				const auto maxAnimInEntity = i + e.nbAnims;
				if (anim < maxAnimInEntity) {
					const auto animIndex = anim - i;
					return entities[e.id].get<AssImpAnimFileComponent>().importer->GetScene()->mAnimations[animIndex];
				}
				i = maxAnimInEntity;
			}

			return nullptr;
		}

		static const aiNodeAnim * getRootNodeAnim(const Entity & modelEntity, const aiAnimation * anim, glm::mat4 & parentTransform) noexcept {
			const auto & assimpSkeleton = modelEntity.get<AssImpModelSkeletonComponent>();
			return findNodeAnimForNode(assimpSkeleton.rootNode, anim, parentTransform, parentTransform);
		}

		static const aiNodeAnim * findNodeAnimForNode(const aiNode * node, const aiAnimation * anim, const glm::mat4 & parentTransform, glm::mat4 & outTotalParentTransform) noexcept {
			auto ret = getNodeAnim(anim, node->mName.C_Str());
			if (ret) {
				outTotalParentTransform = parentTransform;
				return ret;
			}

			const auto totalTransform = parentTransform * helpers::toglmWeird(node->mTransformation);
			for (size_t i = 0; i < node->mNumChildren; ++i) {
				ret = findNodeAnimForNode(node->mChildren[i], anim, totalTransform, outTotalParentTransform);
				if (ret)
					return ret;
			}

			return nullptr;
		}

		static const aiNodeAnim * getNodeAnim(const aiAnimation * anim, const char * name) noexcept {
			for (size_t i = 0; i < anim->mNumChannels; ++i)
				if (strcmp(anim->mChannels[i]->mNodeName.C_Str(), name) == 0)
					return anim->mChannels[i];
			return nullptr;
		}

		template<typename T>
		static unsigned int findPreviousIndex(T * arr, unsigned int size, float time) noexcept {
			for (unsigned i = 0; i < size - 1; ++i) {
				if (time < (float)arr[i + 1].mTime)
					return i;
			}
			return 0;
		}

		static ModelDataComponent::FreeFunc release(EntityID id) noexcept {
			return [id] {
				kengine_logf(Log, "AssimpSystem", "Releasing AssImpModelComponent for %zu", id);
				auto e = entities[id];
				auto & model = e.get<AssImpModelComponent>(); // previous attach hasn't been processed yet, so `get` would assert
				model.importer->FreeScene();
				e.detach<AssImpModelComponent>();
			};
		}

		struct LastFrameMovementComponent {
			unsigned int anim = (unsigned int)-1;
			putils::Point3f pos;
			putils::Vector3f rot;
			putils::Vector3f scale = { 1.f, 1.f, 1.f };
			float time = 0.f;
		};

		static void execute(float deltaTime) noexcept {
			kengine_log(Verbose, "Execute", "AssimpSystem");
			runAnimations(deltaTime);
			if (adjustables.editorMode)
				reloadAnimations();
		}

		static void runAnimations(float deltaTime) noexcept {
			std::atomic<size_t> jobsLeft = 0;

			for (auto [e, instance, skeleton, anim, transform] : entities.with<InstanceComponent, SkeletonComponent, AnimationComponent, TransformComponent>())
			{
				++jobsLeft;

				threadPool().runTask([&, id = e.id]() noexcept {
					const auto cleanup = putils::onScopeExit([&] { --jobsLeft; });

					auto e = entities[id];

					const auto & modelEntity = entities[instance.model];
					if (!modelEntity.has<AssImpModelSkeletonComponent>())
						return;

					auto & lastFrame = e.attach<LastFrameMovementComponent>();

					auto noTranslateTransform = transform;
					noTranslateTransform.boundingBox.position = putils::Point3f{ 0.f, 0.f, 0.f };

					const auto modelMatrix = matrixHelper::getModelMatrix(noTranslateTransform, modelEntity.tryGet<TransformComponent>());

					const auto & modelAnims = modelEntity.get<ModelAnimationComponent>();
					if (anim.currentAnim >= modelAnims.animations.size())
						return;

					const auto assimpAnim = getAnimation(modelEntity, anim.currentAnim);
					kengine_assert(assimpAnim != nullptr);

					if (lastFrame.anim != anim.currentAnim) {
						lastFrame = LastFrameMovementComponent{};
						lastFrame.anim = anim.currentAnim;
						lastFrame.time = anim.currentTime;
						lastFrame.pos = modelAnims.getAnimationMovementUntilTime(e, anim.currentAnim, anim.currentTime);
						lastFrame.rot = modelAnims.getAnimationRotationUntilTime(e, anim.currentAnim, anim.currentTime);
						lastFrame.scale = modelAnims.getAnimationScalingUntilTime(e, anim.currentAnim, anim.currentTime);
					}

					const auto & assimpSkeleton = modelEntity.get<AssImpModelSkeletonComponent>();
					if (skeleton.meshes.empty())
						skeleton.meshes.resize(assimpSkeleton.meshes.size());

					const auto & currentAnim = modelAnims.animations[anim.currentAnim];
					updateBoneMats(assimpSkeleton.rootNode, assimpAnim, anim.currentTime * currentAnim.ticksPerSecond, assimpSkeleton, skeleton, glm::mat4(1.f), anim, transform, lastFrame, modelMatrix, true);

					anim.currentTime += deltaTime * anim.speed;
					const auto wrappedTime = fmodf(anim.currentTime, currentAnim.totalTime);
					if (std::abs(wrappedTime - anim.currentTime) > 0.001f) { // We've looped
						if (anim.loop)
							anim.currentTime = wrappedTime;
						else {
							anim.speed = 0.f;
							anim.currentTime = 0.f;
						}
						lastFrame = {};
					}
				});
			}

			while (jobsLeft > 0);
		}

		static void reloadAnimations() noexcept {
			kengine_log(Verbose, "Execute/Assimp", "Reloading animations for editor mode");
			for (auto [e, model, assimpModel] : entities.with<ModelComponent, AssImpModelComponent>()) {
				kengine_logf(Verbose, "Execute/Assimp/reloadAnimations", "Reloading animations for %zu", e.id);
				loadAnims(e, model.file, assimpModel.importer->GetScene());
			}
		}

		static void updateBoneMats(const aiNode * node, const aiAnimation * anim, float time, const AssImpModelSkeletonComponent & assimpSkeleton, SkeletonComponent & comp, const glm::mat4 & parentTransform, const AnimationComponent & animComponent, TransformComponent & transform, LastFrameMovementComponent & lastFrame, const glm::mat4 & modelMatrix, bool firstNodeAnim) noexcept {
			bool firstCalc = true;
			glm::mat4 totalTransform = parentTransform * helpers::toglmWeird(node->mTransformation);

			const auto nodeAnim = getNodeAnim(anim, node->mName.C_Str());
			if (nodeAnim != nullptr) {
				glm::mat4 mat(1.f);
				const auto pos = calculateInterpolatedPosition(nodeAnim, time);
				const auto rot = calculateInterpolatedRotation(nodeAnim, time);
				const auto scale = calculateInterpolatedScale(nodeAnim, time);

				const auto posInWorldSpace = matrixHelper::convertToReferencial(glm::value_ptr(pos), parentTransform * modelMatrix);
				const auto rotInWorldSpace = matrixHelper::getRotation(glm::mat4_cast(rot));
				const auto scaleInWorldSpace = matrixHelper::convertToReferencial(glm::value_ptr(scale), parentTransform * modelMatrix);

				if (firstNodeAnim) {
					firstNodeAnim = false;

					switch (animComponent.positionMoverBehavior) {
					case AnimationComponent::MoverBehavior::UpdateTransformComponent: {
						const auto movementSinceLastFrame = posInWorldSpace - lastFrame.pos;
						transform.boundingBox.position += movementSinceLastFrame;
						break;
					}
					case AnimationComponent::MoverBehavior::UpdateBones:
						mat = glm::translate(mat, pos);
						break;
					case AnimationComponent::MoverBehavior::None:
					default:
						break;
					}

					switch (animComponent.rotationMoverBehavior) {
					case AnimationComponent::MoverBehavior::UpdateTransformComponent: {
						const auto rotSinceLastFrame = rotInWorldSpace - lastFrame.rot;
						transform.yaw += rotSinceLastFrame.y;
						transform.pitch += rotSinceLastFrame.x;
						transform.roll += rotSinceLastFrame.z;
						break;
					}
					case AnimationComponent::MoverBehavior::UpdateBones:
						mat *= glm::mat4_cast(rot);
						break;
					case AnimationComponent::MoverBehavior::None:
					default:
						break;
					}

					switch (animComponent.scaleMoverBehavior) {
					case AnimationComponent::MoverBehavior::UpdateTransformComponent: {
						const auto scaleSinceLastFrame = scaleInWorldSpace - lastFrame.scale;
						transform.boundingBox.size += scaleSinceLastFrame;
						break;
					}
					case AnimationComponent::MoverBehavior::UpdateBones:
						mat = glm::scale(mat, scale);
						break;
					case AnimationComponent::MoverBehavior::None:
					default:
						break;
					}

					lastFrame.pos = posInWorldSpace;
					lastFrame.rot = rotInWorldSpace;
					lastFrame.scale = scaleInWorldSpace;
				}
				else {
					mat = glm::translate(mat, pos);
					mat *= glm::mat4_cast(rot);
					mat = glm::scale(mat, scale);
				}

				totalTransform = parentTransform * mat;
			}

			for (unsigned int i = 0; i < assimpSkeleton.meshes.size(); ++i) {
				const auto & input = assimpSkeleton.meshes[i];
				auto & output = comp.meshes[i];
				kengine_assert_with_message(input.bones.size() < putils::lengthof(output.boneMatsBoneSpace), "Not enough bones in SkeletonComponent. You need to increase KENGINE_SKELETON_MAX_BONES");

				size_t boneIndex = 0;
				for (const auto & bone : input.bones) {
					if (bone.node == node)
						break;
					++boneIndex;
				}

				if (boneIndex != input.bones.size()) {
					const auto & modelBone = input.bones[boneIndex];
					output.boneMatsMeshSpace[boneIndex] = totalTransform;
					output.boneMatsBoneSpace[boneIndex] = totalTransform * modelBone.offset;
				}
			}

			for (size_t i = 0; i < node->mNumChildren; ++i)
				updateBoneMats(node->mChildren[i], anim, time, assimpSkeleton, comp, totalTransform, animComponent, transform, lastFrame, modelMatrix, firstNodeAnim);
		}

		template<typename T, typename Func>
		static auto calculateInterpolatedValue(T * arr, unsigned int size, float time, Func func) noexcept {
			if (size == 1)
				return helpers::toglm(arr[0].mValue);

			const auto index = findPreviousIndex(arr, size, time);
			const auto & value = arr[index];
			const auto & nextValue = arr[index + 1];

			const auto deltaTime = (float)nextValue.mTime - (float)value.mTime;
			const auto factor = (time - (float)value.mTime) / (float)deltaTime;

			const auto startValue = helpers::toglm(value.mValue);
			const auto endValue = helpers::toglm(nextValue.mValue);

			return func(startValue, endValue, factor);
		}

		static glm::vec3 calculateInterpolatedPosition(const aiNodeAnim * nodeAnim, float time) noexcept {
			return calculateInterpolatedValue(nodeAnim->mPositionKeys, nodeAnim->mNumPositionKeys, time, [](const glm::vec3 & v1, const glm::vec3 & v2, float f) noexcept { return glm::mix(v1, v2, f); });
		}

		static glm::quat calculateInterpolatedRotation(const aiNodeAnim * nodeAnim, float time) noexcept {
			return calculateInterpolatedValue(nodeAnim->mRotationKeys, nodeAnim->mNumRotationKeys, time, glm::slerp<float, glm::defaultp>);
		}

		static glm::vec3 calculateInterpolatedScale(const aiNodeAnim * nodeAnim, float time) noexcept {
			return calculateInterpolatedValue(nodeAnim->mScalingKeys, nodeAnim->mNumScalingKeys, time, [](const glm::vec3 & v1, const glm::vec3 & v2, float f) noexcept { return glm::mix(v1, v2, f); });
		}

		struct helpers {
			static inline Assimp::Importer importer;

			static glm::mat4 toglm(const aiMatrix4x4 & mat) noexcept {
				return glm::make_mat4(&mat.a1);
			}

			static glm::mat4 toglmWeird(const aiMatrix4x4 & mat) noexcept {
				return glm::transpose(toglm(mat));
			}

			static glm::vec3 toglm(const aiVector3D & vec) noexcept {
				return { vec.x, vec.y, vec.z };
			}

			static glm::quat toglm(const aiQuaternion & quat) noexcept {
				return { quat.w, quat.x, quat.y, quat.z };
			}

		};
	};
}

namespace kengine {
	EntityCreator * AssImpSystem() noexcept {
		return [](Entity & e) {
			assimp::impl::init(e);
		};
	}
}