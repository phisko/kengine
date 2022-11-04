#include "KreoglSystem.hpp"
#include "kengine.hpp"

// stl
#include <algorithm>
#include <filesystem>

// gl
#include <GL/glew.h>
#include <GL/gl.h>

// imgui
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

// kreogl
#include "kreogl/Camera.hpp"
#include "kreogl/Sprite.hpp"
#include "kreogl/Text.hpp"
#include "kreogl/Window.hpp"
#include "kreogl/World.hpp"
#include "kreogl/animation/AnimatedObject.hpp"
#include "kreogl/loaders/assimp/AssImp.hpp"

// kengine data
#include "data/AdjustableComponent.hpp"
#include "data/AnimationComponent.hpp"
#include "data/AnimationFilesComponent.hpp"
#include "data/CameraComponent.hpp"
#include "data/DebugGraphicsComponent.hpp"
#include "data/GLFWWindowComponent.hpp"
#include "data/GodRaysComponent.hpp"
#include "data/GraphicsComponent.hpp"
#include "data/ImGuiContextComponent.hpp"
#include "data/ImGuiScaleComponent.hpp"
#include "data/InstanceComponent.hpp"
#include "data/LightComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/ModelAnimationComponent.hpp"
#include "data/ModelDataComponent.hpp"
#include "data/ModelSkeletonComponent.hpp"
#include "data/NoShadowComponent.hpp"
#include "data/SkeletonComponent.hpp"
#include "data/SkyBoxComponent.hpp"
#include "data/SpriteComponent.hpp"
#include "data/TextComponent.hpp"
#include "data/TransformComponent.hpp"
#include "data/ViewportComponent.hpp"
#include "data/WindowComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"
#include "functions/GetEntityInPixel.hpp"
#include "functions/GetPositionInPixel.hpp"

// kengine helpers
#include "helpers/cameraHelper.hpp"
#include "helpers/imguiHelper.hpp"
#include "helpers/instanceHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/matrixHelper.hpp"
#include "helpers/profilingHelper.hpp"

// impl
#include "KreoglAnimationFilesComponent.hpp"
#include "KreoglDebugGraphicsComponent.hpp"
#include "KreoglModelComponent.hpp"
#include "shaders/HighlightShader.hpp"
#include "PutilsToGLM.hpp"

namespace kengine {
	struct KreoglSystem {
		static void init(Entity & e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Log, "Init", "KreoglSystem");

			e += functions::Execute{ execute };

			auto & scale = e.attach<ImGuiScaleComponent>();
			e += AdjustableComponent{
				"ImGui", {
					{ "Scale", &scale.scale }
				}
			};

			e += functions::GetEntityInPixel{ getEntityInPixel };
			e += functions::GetPositionInPixel{ getPositionInPixel };

			initWindow();
		}

		static void initWindow() noexcept {
			KENGINE_PROFILING_SCOPE;

			EntityID windowID = INVALID_ID;

			// Find potential existing window
			for (const auto & [e, w] : entities.with<WindowComponent>()) {
				if (!w.assignedSystem.empty())
					continue;
				kengine_logf(Log, "Init/KreoglSystem", "Found existing window: %zu", e.id);
				windowID = e.id;
				break;
			}

			// If none found, create one
			if (windowID == INVALID_ID)
				entities += [&](Entity & e) noexcept {
					kengine_logf(Log, "Init/KreoglSystem", "Created default window: %zu", e.id);
					auto & windowComp = e.attach<WindowComponent>();
					windowComp.name = "Kengine";
					windowID = e.id;
				};

			// Ask the GLFW system to initialize the window
			auto windowEntity = entities[windowID];
			windowEntity.get<WindowComponent>().assignedSystem = "Kreogl";
			windowEntity += GLFWWindowInitComponent{
				.setHints = []() noexcept {
					kengine_log(Log, "Init/KreoglSystem", "Setting window hints");
					glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
					glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
					glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
					glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#ifndef KENGINE_NDEBUG
					glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
				},
				.onWindowCreated = [windowID]() noexcept {
					kengine_log(Log, "Init/KreoglSystem", "GLFW window created");
					kengine_log(Log, "Init/KreoglSystem", "Creating kreogl window");
					const auto windowEntity = entities[windowID];
					const auto & glfwWindow = windowEntity.get<GLFWWindowComponent>();
					auto & kreoglWindow = entities[windowID].attach(kreogl::Window{ *glfwWindow.window.get() });
					kreoglWindow.removeCamera(kreoglWindow.getDefaultCamera());
					initImGui(windowID);
				}
			};
		}

		static void initImGui(EntityID windowID) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Log, "Init/KreoglSystem", "Initializing ImGui");

			auto windowEntity = entities[windowID];
			windowEntity += ImGuiContextComponent{
				ImGui::CreateContext()
			};

			auto & io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			io.ConfigViewportsNoTaskBarIcon = true;

			const auto & glfwComp = windowEntity.get<GLFWWindowComponent>();
			ImGui_ImplGlfw_InitForOpenGL(glfwComp.window.get(), true);
			ImGui_ImplOpenGL3_Init();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Verbose, "Execute", "KreoglSystem");

			createMissingModels();
			createMissingObjects();
			tickAnimations(deltaTime);

			updateImGuiScale();
			draw();
			endImGuiFrame();
		}

		static void updateImGuiScale() noexcept {
			KENGINE_PROFILING_SCOPE;

			static float lastScale = 1.f;
			const auto scale = imguiHelper::getScale();
			ImGui::GetIO().FontGlobalScale = scale;
			ImGui::GetStyle().ScaleAllSizes(scale / lastScale);
			lastScale = scale;
		}

		static void createMissingModels() noexcept {
			KENGINE_PROFILING_SCOPE;

			createModelsFromDisk();
			createModelsFromModelData();
		}

		static void createModelsFromDisk() noexcept {
			KENGINE_PROFILING_SCOPE;

			for (auto [modelEntity, model, noKreoglModel, noKreoglImageTexture] : entities.with<ModelComponent, no<KreoglModelComponent>, no<kreogl::ImageTexture>>()) {
				kengine_logf(Verbose, "Execute/KreoglSystem", "Creating Kreogl model for %zu (%s)", modelEntity.id, model.file.c_str());
				if (kreogl::ImageTexture::isSupportedFormat(model.file.c_str()))
					modelEntity += kreogl::ImageTexture(model.file.c_str());
				else if (kreogl::AssImp::isSupportedFileFormat(model.file.c_str()))
					createModelWithAssImp(modelEntity, model);
			}
		}

		static void createModelWithAssImp(Entity & modelEntity, const ModelComponent & model) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto animatedModel = kreogl::AssImp::loadAnimatedModel(model.file.c_str());
			if (animatedModel) {
				// Sync properties from loaded model to kengine components
				addAnimationsToModelAnimationComponent(modelEntity.attach<ModelAnimationComponent>(), model.file.c_str(), animatedModel->animations);
				if (animatedModel->skeleton) {
					auto & modelSkeleton = modelEntity.attach<ModelSkeletonComponent>();
					for (const auto & mesh : animatedModel->skeleton->meshes)
						modelSkeleton.meshes.push_back(ModelSkeletonComponent::Mesh{
							.boneNames = mesh.boneNames
						});
				}
			}
			modelEntity += KreoglModelComponent{ std::move(animatedModel) };

			// Load animations from external files
			if (const auto animationFiles = modelEntity.tryGet<AnimationFilesComponent>()) {
				auto & modelAnimation = modelEntity.get<ModelAnimationComponent>();
				auto & kreoglAnimationFiles = modelEntity.attach<KreoglAnimationFilesComponent>();
				for (const auto & file : animationFiles->files) {
					auto kreoglAnimationFile = kreogl::AssImp::loadAnimationFile(file.c_str());
					addAnimationsToModelAnimationComponent(modelAnimation, file.c_str(), kreoglAnimationFile->animations);
					kreoglAnimationFiles.files.push_back(std::move(kreoglAnimationFile));
				}
			}
		}

		static void addAnimationsToModelAnimationComponent(ModelAnimationComponent & modelAnimation, const char * file, const std::vector<std::unique_ptr<kreogl::AnimationModel>> & animations) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & animation : animations) {
				std::string name = file;
				name += '/';
				name += animation->name;
				modelAnimation.animations.push_back(ModelAnimationComponent::Anim{
					.name = name,
					.totalTime = animation->totalTime,
					.ticksPerSecond = animation->ticksPerSecond
				});
			}
		}

		static void createModelsFromModelData() noexcept {
			KENGINE_PROFILING_SCOPE;

			for (auto [modelEntity, modelData, noKreoglModel] : entities.with<ModelDataComponent, no<KreoglModelComponent>>()) {
				kreogl::ModelData kreoglModelData;

				for (const auto & meshData : modelData.meshes) {
					static const std::unordered_map<putils::meta::type_index, GLenum> types = {
						{ putils::meta::type<char>::index, GL_BYTE },
						{ putils::meta::type<unsigned char>::index, GL_UNSIGNED_BYTE },
						{ putils::meta::type<short>::index, GL_SHORT },
						{ putils::meta::type<unsigned short>::index, GL_UNSIGNED_SHORT },
						{ putils::meta::type<int>::index, GL_INT },
						{ putils::meta::type<unsigned int>::index, GL_UNSIGNED_INT },
						{ putils::meta::type<float>::index, GL_FLOAT },
						{ putils::meta::type<double>::index, GL_DOUBLE }
					};

					kreoglModelData.meshes.push_back(kreogl::MeshData{
						.vertices = {
							.nbElements = meshData.vertices.nbElements,
							.elementSize = meshData.vertices.elementSize,
							.data = meshData.vertices.data
						},
						.indices = {
							.nbElements = meshData.indices.nbElements,
							.elementSize = meshData.indices.elementSize,
							.data = meshData.indices.data
						},
						.indexType = types.at(meshData.indexType)
					});
				}

				for (const auto & vertexAttribute : modelData.vertexAttributes)
					kreoglModelData.vertexAttributeOffsets.push_back(vertexAttribute.offset);

				kreoglModelData.vertexSize = modelData.vertexSize;

				// We assume custom-built meshes are voxels, might need a better alternative to this
				const auto & vertexSpecification = kreogl::VertexSpecification::positionColor;
				modelEntity += KreoglModelComponent{ std::make_unique<kreogl::AnimatedModel>(vertexSpecification, kreoglModelData) };
			}
		}

		static void createMissingObjects() noexcept {
			KENGINE_PROFILING_SCOPE;

			for (auto [entity, instance, noKreoglObject, noKreoglSprite2D, noKreoglSprite3D] : entities.with<InstanceComponent, no<kreogl::AnimatedObject>, no<kreogl::Sprite2D>, no<kreogl::Sprite3D>>()) {
				if (instance.model == INVALID_ID)
					continue;

				const auto modelEntity = entities[instance.model];

				if (const auto kreoglModel = modelEntity.tryGet<KreoglModelComponent>()) {
					entity.attach<kreogl::AnimatedObject>().model = kreoglModel->model.get();
					if (modelEntity.has<ModelSkeletonComponent>())
						entity += SkeletonComponent{};
				}

				if (const auto kreoglTexture = modelEntity.tryGet<kreogl::ImageTexture>()) {
					if (entity.has<SpriteComponent2D>())
						entity.attach<kreogl::Sprite2D>().texture = kreoglTexture;
					if (entity.has<SpriteComponent3D>())
						entity.attach<kreogl::Sprite3D>().texture = kreoglTexture;
				}
			}

			for (auto [text2DEntity, text2D, noKreoglText2D] : entities.with<TextComponent2D, no<kreogl::Text2D>>())
				text2DEntity += kreogl::Text2D{};

			for (auto [text3DEntity, text3D, noKreoglText3D] : entities.with<TextComponent3D, no<kreogl::Text3D>>())
				text3DEntity += kreogl::Text3D{};

			for (auto [lightEntity, dirLight, noKreoglDirLight] : entities.with<DirLightComponent, no<kreogl::DirectionalLight>>())
				lightEntity += kreogl::DirectionalLight{};

			for (auto [lightEntity, dirLight, noKreoglPointLight] : entities.with<PointLightComponent, no<kreogl::PointLight>>())
				lightEntity += kreogl::PointLight{};

			for (auto [lightEntity, dirLight, noKreoglSpotLight] : entities.with<SpotLightComponent, no<kreogl::SpotLight>>())
				lightEntity += kreogl::SpotLight{};

			for (auto [skyboxEntity, skyBox, noKreoglSkybox] : entities.with<SkyBoxComponent, no<kreogl::SkyboxTexture>>())
				skyboxEntity += kreogl::SkyboxTexture(skyBox.left.c_str(), skyBox.right.c_str(), skyBox.top.c_str(), skyBox.bottom.c_str(), skyBox.front.c_str(), skyBox.back.c_str());

			for (auto [debugEntity, debugGraphics, noKreoglDebugGraphics] : entities.with<DebugGraphicsComponent, no<KreoglDebugGraphicsComponent>>())
				debugEntity += KreoglDebugGraphicsComponent{};
		}

		static void tickAnimations(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (auto [entity, kreoglObject, animation] : entities.with<kreogl::AnimatedObject, AnimationComponent>()) {
				if (!kreoglObject.animation)
					continue;

				kreoglObject.tickAnimation(deltaTime);

				// Sync properties from kreoglObject to kengine components
				animation.currentTime = kreoglObject.animation->currentTime;

				auto & skeleton = entity.get<SkeletonComponent>();
				const auto nbMeshes = kreoglObject.skeleton.meshes.size();
				skeleton.meshes.resize(nbMeshes);
				for (size_t i = 0; i < nbMeshes; ++i) {
					const auto & kreoglMesh = kreoglObject.skeleton.meshes[i];
					auto & mesh = skeleton.meshes[i];

					kengine_assert(kreoglMesh.boneMatsBoneSpace.size() < putils::lengthof(mesh.boneMatsBoneSpace));
					std::ranges::copy(kreoglMesh.boneMatsBoneSpace, mesh.boneMatsBoneSpace);
					kengine_assert(kreoglMesh.boneMatsMeshSpace.size() < putils::lengthof(mesh.boneMatsMeshSpace));
					std::ranges::copy(kreoglMesh.boneMatsMeshSpace, mesh.boneMatsMeshSpace);
				}
			}
		}

		static void draw() noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [windowEntity, kreoglWindow] : entities.with<kreogl::Window>()) {
				kreoglWindow.prepareForDraw();
				drawToCameras(windowEntity, kreoglWindow);

				ImGui::Render();
				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
				kreoglWindow.display();
			}
		}

		static void drawToCameras(const Entity & windowEntity, kreogl::Window & kreoglWindow) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (auto [cameraEntity, camera, viewport] : entities.with<CameraComponent, ViewportComponent>()) {
				if (viewport.window != INVALID_ID && viewport.window != windowEntity.id)
					continue;

				if (viewport.window == INVALID_ID) {
					kengine_logf(Log, "KreoglSystem", "Setting target window for ViewportComponent in %zu", cameraEntity.id);
					viewport.window = windowEntity.id;
					createKreoglCamera(cameraEntity, viewport);
				}

				auto & kreoglCamera = cameraEntity.get<kreogl::Camera>();
				syncCameraProperties(kreoglCamera, camera, viewport);
				drawToCamera(kreoglWindow, cameraEntity, kreoglCamera);
			}
		}

		static void createKreoglCamera(Entity & cameraEntity, ViewportComponent & viewport) noexcept {
			KENGINE_PROFILING_SCOPE;

			const kreogl::Camera::ConstructionParams params{
				.viewport = {
					.resolution = toglm(viewport.resolution)
				}
			};
			auto & kreoglCamera = cameraEntity.attach<kreogl::Camera>(params);
			viewport.renderTexture = ViewportComponent::RenderTexture(kreoglCamera.getViewport().getRenderTexture());
		}

		static void syncCameraProperties(kreogl::Camera & kreoglCamera, const CameraComponent & camera, const ViewportComponent & viewport) noexcept {
			KENGINE_PROFILING_SCOPE;

			kreoglCamera.setPosition(toglm(camera.frustum.position));

			const auto facings = cameraHelper::getFacings(camera);
			kreoglCamera.setDirection(toglm(facings.front));
			kreoglCamera.setFOV(camera.frustum.size.y);
			kreoglCamera.setNearPlane(camera.nearPlane);
			kreoglCamera.setFarPlane(camera.farPlane);

			auto & kreoglViewport = kreoglCamera.getViewportWritable();
			kreoglViewport.setOnScreenPosition(toglm(viewport.boundingBox.position));
			kreoglViewport.setOnScreenSize(toglm(viewport.boundingBox.size));
			kreoglViewport.setResolution(toglm(viewport.resolution));
			kreoglViewport.setZOrder(viewport.zOrder);
		}

		static void drawToCamera(kreogl::Window & kreoglWindow, const Entity & cameraEntity, const kreogl::Camera & kreoglCamera) noexcept {
			KENGINE_PROFILING_SCOPE;

			kreogl::World kreoglWorld;
			syncEverything(kreoglWorld, cameraEntity);
			kreoglWindow.drawWorldToCamera(kreoglWorld, kreoglCamera, getShaderPipeline());
		}


		static void syncEverything(kreogl::World & kreoglWorld, const Entity & cameraEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			syncAllObjects(kreoglWorld, cameraEntity);
			syncAllLights(kreoglWorld, cameraEntity);
		}

		static void syncCommonProperties(auto & kreoglObject, const Entity & entity, const InstanceComponent * instance, const TransformComponent & transform, const auto & coloredComponent, kreogl::World & kreoglWorld) noexcept {
			KENGINE_PROFILING_SCOPE;

			const TransformComponent * modelTransform = nullptr;
			if (instance)
				modelTransform = instanceHelper::tryGetModel<TransformComponent>(*instance);
			kreoglObject.transform = matrixHelper::getModelMatrix(transform, modelTransform);
			kreoglObject.color = toglm(coloredComponent.color);
			kreoglObject.userData[0] = float(entity.id);

			kreoglWorld.add(kreoglObject);
		};

		static void syncAllObjects(kreogl::World & kreoglWorld, const Entity & cameraEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [entity, instance, transform, graphics, kreoglObject] : entities.with<InstanceComponent, TransformComponent, GraphicsComponent, kreogl::AnimatedObject>()) {
				if (!cameraHelper::entityAppearsInViewport(entity, cameraEntity))
					continue;
				syncCommonProperties(kreoglObject, entity, &instance, transform, graphics, kreoglWorld);
				syncAnimationProperties(kreoglObject, entity, instance);
				kreoglObject.castShadows = !entity.has<NoShadowComponent>();
			}

			syncSprite2DProperties(kreoglWorld, cameraEntity);
			for (const auto & [spriteEntity, instance, transform, graphics, sprite3D, kreoglSprite3D] : entities.with<InstanceComponent, TransformComponent, GraphicsComponent, SpriteComponent3D, kreogl::Sprite3D>()) {
				if (!cameraHelper::entityAppearsInViewport(spriteEntity, cameraEntity))
					continue;
				syncCommonProperties(kreoglSprite3D, spriteEntity, &instance, transform, graphics, kreoglWorld);
			}

			syncText2DProperties(kreoglWorld, cameraEntity);
			for (const auto & [textEntity, transform, text3D, kreoglText3D] : entities.with<TransformComponent, TextComponent3D, kreogl::Text3D>()) {
				if (!cameraHelper::entityAppearsInViewport(textEntity, cameraEntity))
					continue;
				syncTextProperties(kreoglText3D, textEntity, transform, text3D, kreoglWorld);
			}

			syncDebugGraphicsProperties(kreoglWorld, cameraEntity);

			for (const auto & [skyboxEntity, skyBox, kreoglSkybox] : entities.with<SkyBoxComponent, kreogl::SkyboxTexture>()) {
				if (!cameraHelper::entityAppearsInViewport(skyboxEntity, cameraEntity))
					continue;
				kreoglWorld.skybox.color = toglm(skyBox.color);
				kreoglWorld.skybox.texture = &kreoglSkybox;
			}
		}

		static void syncAnimationProperties(kreogl::AnimatedObject & kreoglObject, const Entity & entity, const InstanceComponent & instance) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto animation = entity.tryGet<AnimationComponent>();
			if (!animation)
				return;

			const auto animationModel = findAnimationModel(instance, animation->currentAnim);
			if (!animationModel)
				kreoglObject.animation.reset();
			else {
				kreoglObject.animation = kreogl::Animation{
					.model = animationModel,
					.currentTime = animation->currentTime,
					.speed = animation->speed,
					.loop = animation->loop
				};

				static const std::unordered_map<AnimationComponent::MoverBehavior, kreogl::Animation::MoverBehavior> moverBehaviors = {
					{ AnimationComponent::MoverBehavior::UpdateTransformComponent, kreogl::Animation::MoverBehavior::UpdateTransform },
					{ AnimationComponent::MoverBehavior::UpdateBones, kreogl::Animation::MoverBehavior::UpdateBones },
					{ AnimationComponent::MoverBehavior::None, kreogl::Animation::MoverBehavior::None }
				};

				kreoglObject.animation->positionMoverBehavior = moverBehaviors.at(animation->positionMoverBehavior);
				kreoglObject.animation->rotationMoverBehavior = moverBehaviors.at(animation->rotationMoverBehavior);
				kreoglObject.animation->scaleMoverBehavior = moverBehaviors.at(animation->scaleMoverBehavior);
			}
		}

		static const kreogl::AnimationModel * findAnimationModel(const InstanceComponent & instance, size_t animationIndex) {
			KENGINE_PROFILING_SCOPE;

			const auto & kreoglModel = instanceHelper::getModel<KreoglModelComponent>(instance);
			if (animationIndex < kreoglModel.model->animations.size())
				return kreoglModel.model->animations[animationIndex].get();

			const auto kreoglModelAnimationFiles = instanceHelper::tryGetModel<KreoglAnimationFilesComponent>(instance);
			if (!kreoglModelAnimationFiles)
				return nullptr;

			size_t skippedAnimations = kreoglModel.model->animations.size();
			for (const auto & file : kreoglModelAnimationFiles->files) {
				const auto indexInFile = animationIndex - skippedAnimations;
				if (indexInFile < file->animations.size())
					return file->animations[indexInFile].get();
				skippedAnimations += file->animations.size();
			}

			return nullptr;
		}

		static void syncSprite2DProperties(kreogl::World & kreoglWorld, const Entity & cameraEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [spriteEntity, instance, transform, graphics, sprite2D, kreoglSprite2D] : entities.with<InstanceComponent, TransformComponent, GraphicsComponent, SpriteComponent2D, kreogl::Sprite2D>()) {
				if (!cameraHelper::entityAppearsInViewport(spriteEntity, cameraEntity))
					continue;

				syncCommonProperties(kreoglSprite2D, spriteEntity, &instance, transform, graphics, kreoglWorld);

				const auto & viewport = cameraEntity.get<ViewportComponent>();
				const auto & box = cameraHelper::convertToScreenPercentage(transform.boundingBox, viewport.resolution, sprite2D);
				kreoglSprite2D.transform = getOnScreenMatrix(transform, box.position, box.size, sprite2D);
			}
		}

		static void syncText2DProperties(kreogl::World & kreoglWorld, const Entity & cameraEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [textEntity, transform, text2D, kreoglText2D] : entities.with<TransformComponent, TextComponent2D, kreogl::Text2D>()) {
				if (!cameraHelper::entityAppearsInViewport(textEntity, cameraEntity))
					continue;

				syncTextProperties(kreoglText2D, textEntity, transform, text2D, kreoglWorld);

				const auto & viewport = cameraEntity.get<ViewportComponent>();
				const auto & box = cameraHelper::convertToScreenPercentage(transform.boundingBox, viewport.resolution, text2D);
				auto scale = transform.boundingBox.size.y;
				switch (text2D.coordinateType) {
					case TextComponent2D::CoordinateType::Pixels:
						scale /= viewport.resolution.y;
						break;
					case TextComponent2D::CoordinateType::ScreenPercentage:
					default:
						static_assert(magic_enum::enum_count<TextComponent2D::CoordinateType>() == 2);
						break;
				}
				kreoglText2D.transform = getOnScreenMatrix(transform, box.position, { scale, scale, scale }, text2D);
			}
		}

		static void syncDebugGraphicsProperties(kreogl::World & kreoglWorld, const Entity & cameraEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [debugEntity, transform, debugGraphics, kreoglDebugGraphics] : entities.with<TransformComponent, DebugGraphicsComponent, KreoglDebugGraphicsComponent>()) {
				if (!cameraHelper::entityAppearsInViewport(debugEntity, cameraEntity))
					continue;

				kreoglDebugGraphics.elements.resize(debugGraphics.elements.size());
				for (size_t i = 0; i < debugGraphics.elements.size(); ++i) {
					const auto & element = debugGraphics.elements[i];
					auto & kreoglElement = kreoglDebugGraphics.elements[i];
					kreoglElement.color = toglm(element.color);
					kreoglElement.userData[0] = float(debugEntity.id);

					kreoglElement.transform = glm::mat4{ 1.f };
					switch (element.type) {
						case DebugGraphicsComponent::Type::Line:
							kreoglElement.type = kreogl::DebugElement::Type::Line;
							kreoglElement.lineStart = toglm(element.pos);
							kreoglElement.lineEnd = toglm(element.line.end);
							break;
						case DebugGraphicsComponent::Type::Box:
							kreoglElement.type = kreogl::DebugElement::Type::Box;
							applyDebugGraphicsTransform(kreoglElement.transform, transform, toglm(element.pos), toglm(element.box.size), element.referenceSpace);
							break;
						case DebugGraphicsComponent::Type::Sphere:
							kreoglElement.type = kreogl::DebugElement::Type::Sphere;
							applyDebugGraphicsTransform(kreoglElement.transform, transform, toglm(element.pos), glm::vec3(element.sphere.radius), element.referenceSpace);
							break;
						default:
							static_assert(magic_enum::enum_count<DebugGraphicsComponent::Type>() == 3); // Exhaustive switch
							kengine_assert_failed("Non-exhaustive switch");
							break;
					}

					kreoglWorld.add(kreoglElement);
				}
			}
		}

		static void applyDebugGraphicsTransform(glm::mat4 & matrix, const TransformComponent & transform, const glm::vec3 & pos, const glm::vec3 & size, DebugGraphicsComponent::ReferenceSpace referenceSpace) noexcept {
			KENGINE_PROFILING_SCOPE;

			matrix = glm::translate(matrix, pos);
			if (referenceSpace == DebugGraphicsComponent::ReferenceSpace::Object) {
				matrix = glm::translate(matrix, toglm(transform.boundingBox.position));
				matrix = glm::rotate(matrix, transform.yaw, {0.f, 1.f, 0.f});
				matrix = glm::rotate(matrix, transform.pitch, {1.f, 0.f, 0.f});
				matrix = glm::rotate(matrix, transform.roll, {0.f, 0.f, 1.f});
			}
			matrix = glm::scale(matrix, size);
			if (referenceSpace == DebugGraphicsComponent::ReferenceSpace::Object)
				matrix = glm::scale(matrix, toglm(transform.boundingBox.size));
		}

		static glm::mat4 getOnScreenMatrix(const TransformComponent & transform, const putils::Point3f & position, const putils::Point3f & size, const OnScreenComponent & onScreen) noexcept {
			KENGINE_PROFILING_SCOPE;

			glm::mat4 model{ 1.f };
			// convert NDC to [0,1]
			model = glm::translate(model, { -1.f, -1.f, 0.f });
			const auto centre = 2.f * glm::vec3{ position.x, 1.f - position.y, position.z };

			model = glm::translate(model, centre);
			model = glm::scale(model, toglm(size));
			model = glm::rotate(model, transform.yaw, { 0.f, 1.f, 0.f });
			model = glm::rotate(model, transform.pitch, { 1.f, 0.f, 0.f });
			model = glm::rotate(model, transform.roll, { 0.f, 0.f, 1.f });
			return model;
		}

		static void syncTextProperties(auto & kreoglText, const Entity & textEntity, const TransformComponent & transform, const TextComponent & text, kreogl::World & kreoglWorld) noexcept {
			KENGINE_PROFILING_SCOPE;

			syncCommonProperties(kreoglText, textEntity, nullptr, transform, text, kreoglWorld);
			kreoglText.font = text.font;
			kreoglText.text = text.text;
			kreoglText.fontSize = text.fontSize;
			switch (text.alignment) {
				case TextComponent::Alignment::Left:
					kreoglText.alignment = kreogl::Text::Alignment::Left;
					break;
				case TextComponent::Alignment::Right:
					kreoglText.alignment = kreogl::Text::Alignment::Right;
					break;
				case TextComponent::Alignment::Center:
					kreoglText.alignment = kreogl::Text::Alignment::Center;
					break;
				default:
					static_assert(magic_enum::enum_count<TextComponent::Alignment>() == 3); // Exhaustive switch
					kengine_assert_failed("Non-exhaustive switch");
					break;
			}
		}

		static void syncAllLights(kreogl::World & kreoglWorld, const Entity & cameraEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			syncAllDirLights(kreoglWorld, cameraEntity);
			syncAllPointLights(kreoglWorld, cameraEntity);
			syncAllSpotLights(kreoglWorld, cameraEntity);
		}

		static void syncAllDirLights(kreogl::World & kreoglWorld, const Entity & cameraEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [lightEntity, dirLight, kreoglDirLight] : entities.with<DirLightComponent, kreogl::DirectionalLight>()) {
				if (!cameraHelper::entityAppearsInViewport(lightEntity, cameraEntity))
					continue;

				syncLightProperties(lightEntity, kreoglDirLight, dirLight, kreoglWorld);

				kreoglDirLight.direction = toglm(dirLight.direction);
				kreoglDirLight.lightSphereDistance = dirLight.lightSphereDistance;
				kreoglDirLight.ambientStrength = dirLight.ambientStrength;

				kreoglDirLight.cascadeEnds.clear();
				for (const auto cascadeEnd : dirLight.cascadeEnds)
					kreoglDirLight.cascadeEnds.push_back(cascadeEnd);
				kreoglDirLight.shadowCasterMaxDistance = dirLight.shadowCasterMaxDistance;
			}
		}

		static void syncAllPointLights(kreogl::World & kreoglWorld, const Entity & cameraEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [lightEntity, transform, pointLight, kreoglPointLight] : entities.with<TransformComponent, PointLightComponent, kreogl::PointLight>()) {
				if (!cameraHelper::entityAppearsInViewport(lightEntity, cameraEntity))
					continue;
				syncPointLightProperties(lightEntity, transform, kreoglPointLight, pointLight, kreoglWorld);
			}
		}

		static void syncAllSpotLights(kreogl::World & kreoglWorld, const Entity & cameraEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [lightEntity, transform, spotLight, kreoglSpotLight] : entities.with<TransformComponent, SpotLightComponent, kreogl::SpotLight>()) {
				if (!cameraHelper::entityAppearsInViewport(lightEntity, cameraEntity))
					continue;

				syncPointLightProperties(lightEntity, transform, kreoglSpotLight, spotLight, kreoglWorld);

				kreoglSpotLight.direction = toglm(spotLight.direction);
				kreoglSpotLight.cutOff = spotLight.cutOff;
				kreoglSpotLight.outerCutOff = spotLight.outerCutOff;
			}
		}

		static void syncLightProperties(const Entity & lightEntity, auto & kreoglLight, const LightComponent & light, kreogl::World & kreoglWorld) noexcept {
			KENGINE_PROFILING_SCOPE;

			kreoglLight.color = toglm(light.color);
			kreoglLight.diffuseStrength = light.diffuseStrength;
			kreoglLight.specularStrength = light.specularStrength;
			kreoglLight.castShadows = light.castShadows;
			kreoglLight.shadowPCFSamples = light.shadowPCFSamples;
			kreoglLight.shadowMapSize = light.shadowMapSize;
			kreoglLight.shadowMapMaxBias = light.shadowMapMaxBias;
			kreoglLight.shadowMapMinBias = light.shadowMapMinBias;

			if (const auto godRays = lightEntity.tryGet<GodRaysComponent>()) {
				if (!kreoglLight.volumetricLighting)
					kreoglLight.volumetricLighting.emplace();
				kreoglLight.volumetricLighting->defaultStepLength = godRays->defaultStepLength;
				kreoglLight.volumetricLighting->intensity = godRays->intensity;
				kreoglLight.volumetricLighting->nbSteps = godRays->nbSteps;
				kreoglLight.volumetricLighting->scattering = godRays->scattering;
			}
			else
				kreoglLight.volumetricLighting.reset();

			kreoglWorld.add(kreoglLight);
		}

		static void syncPointLightProperties(const Entity & lightEntity, const TransformComponent & transform, kreogl::PointLight & kreoglLight, const PointLightComponent & light, kreogl::World & kreoglWorld) noexcept {
			KENGINE_PROFILING_SCOPE;

			syncLightProperties(lightEntity, kreoglLight, light, kreoglWorld);

			kreoglLight.position = toglm(transform.boundingBox.position);
			kreoglLight.range = light.range;
			kreoglLight.attenuationConstant = light.attenuationConstant;
			kreoglLight.attenuationLinear = light.attenuationLinear;
			kreoglLight.attenuationQuadratic = light.attenuationQuadratic;
		}

		static void endImGuiFrame() noexcept {
			KENGINE_PROFILING_SCOPE;

			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				GLFWwindow * backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		static const kreogl::ShaderPipeline & getShaderPipeline() noexcept {
			KENGINE_PROFILING_SCOPE;

			static const auto shaderPipeline = [] {
				auto ret = kreogl::ShaderPipeline::getDefaultShaders();
				ret.addShader(kreogl::ShaderStep::PostProcess, HighlightShader::getSingleton());
				return ret;
			}();
			return shaderPipeline;
		}

		static EntityID getEntityInPixel(EntityID windowID, const putils::Point2ui & pixel) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(Verbose, "KreoglSystem", "Getting entity in { %zu, %zu } of %zu", pixel.x, pixel.y, windowID);

			const auto entityIDInPixel = readFromGBuffer(windowID, pixel, kreogl::GBuffer::Texture::UserData);
			if (!entityIDInPixel)
				return INVALID_ID;

			const auto ret = EntityID(entityIDInPixel->r);
			if (ret == 0) {
				kengine_log(Verbose, "OpenGLSystem/getEntityInPixel", "Found no Entity");
				return INVALID_ID;
			}

			kengine_logf(Verbose, "OpenGLSystem/getEntityInPixel", "Found %zu", ret);
			return ret;
		}

		static std::optional<putils::Point3f> getPositionInPixel(EntityID windowID, const putils::Point2ui & pixel) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(Verbose, "KreoglSystem", "Getting position in { %zu, %zu } of %zu", pixel.x, pixel.y, windowID);

			// Check that an entity was drawn in the pixel, otherwise position buffer is invalid
			if (getEntityInPixel(windowID, pixel) == INVALID_ID)
				return std::nullopt;

			const auto positionInPixel = readFromGBuffer(windowID, pixel, kreogl::GBuffer::Texture::Position);
			if (!positionInPixel)
				return std::nullopt;

			kengine_logf(Verbose, "KreoglSystem/getPositionInPixel", "Found { %f, %f, %f }", positionInPixel->x, positionInPixel->y, positionInPixel->y);
			return &positionInPixel->x;
		}

		static std::optional<glm::vec4> readFromGBuffer(EntityID windowID, const putils::Point2ui & pixel, kreogl::GBuffer::Texture texture) noexcept {
			KENGINE_PROFILING_SCOPE;

			static constexpr auto GBUFFER_TEXTURE_COMPONENTS = 4;

			if (windowID == INVALID_ID) {
				for (const auto & [windowEntity, kreoglWindow] : entities.with<kreogl::Window>()) {
					windowID = windowEntity.id;
					break;
				}

				if (windowID == INVALID_ID) {
					kengine_log(Warning, "KreoglSystem", "No existing Kreogl window");
					return std::nullopt;
				}
			}

			const auto windowEntity = entities[windowID];
			if (!windowEntity.has<kreogl::Window>()) {
				kengine_logf(Warning, "KreoglSystem", "%zu does not have a Kreogl window", windowID);
				return std::nullopt;
			}

			const auto viewportInfo = cameraHelper::getViewportForPixel(windowID, pixel);
			if (viewportInfo.camera == INVALID_ID) {
				kengine_log(Warning, "KreoglSystem", "Found no viewport containing pixel");
				return std::nullopt;
			}

			const auto cameraEntity = entities[viewportInfo.camera];
			const auto kreoglCamera = cameraEntity.tryGet<kreogl::Camera>();
			if (!kreoglCamera) {
				kengine_logf(Warning, "KreoglSystem", "Viewport %zu does not have a kreogl::Camera", cameraEntity.id);
				return std::nullopt;
			}

			const auto & viewport = kreoglCamera->getViewport();
			const auto & gBuffer = viewport.getGBuffer();
			const auto gBufferSize = viewport.getResolution();

			const auto pixelInGBuffer = glm::vec2(gBufferSize) * toglm(viewportInfo.viewportPercent);
			if (pixelInGBuffer.x >= float(gBufferSize.x) || pixelInGBuffer.y > float(gBufferSize.y) || pixelInGBuffer.y == 0) {
				kengine_logf(Warning, "OpenGLSystem/getEntityInPixel", "Pixel is out of %zu's GBuffer's bounds", cameraEntity.id);
				return std::nullopt;
			}

			return gBuffer.readPixel(texture, pixelInGBuffer);
		}
	};

	EntityCreator * KreoglSystem() noexcept {
		return KreoglSystem::init;
	}
}