#include "KreoglSystem.hpp"

// stl
#include <algorithm>
#include <atomic>
#include <execution>
#include <filesystem>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// gl
#include <GL/glew.h>
#include <GL/gl.h>

// imgui
#include <imgui.h>
#include <bindings/imgui_impl_glfw.h>
#include <bindings/imgui_impl_opengl3.h>

// putils
#include "forward_to.hpp"

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
#include "data/KeepAlive.hpp"
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
#include "functions/OnTerminate.hpp"

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
		entt::registry & r;

		KreoglSystem(entt::handle e) noexcept
			: r(*e.registry())
		{
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "KreoglSystem");

			e.emplace<functions::Execute>(putils_forward_to_this(execute));
			e.emplace<functions::OnTerminate>(putils_forward_to_this(terminate));

			auto & scale = e.emplace<ImGuiScaleComponent>();
			e.emplace<AdjustableComponent>() = {
				"ImGui", {
					{ "Scale", &scale.scale }
				}
			};

			e.emplace<functions::GetEntityInPixel>(putils_forward_to_this(getEntityInPixel));
			e.emplace<functions::GetPositionInPixel>(putils_forward_to_this(getPositionInPixel));

			initWindow();
		}

		void initWindow() noexcept {
			KENGINE_PROFILING_SCOPE;

			entt::entity windowEntity = entt::null;

			// Find potential existing window
			for (const auto & [e, w] : r.view<WindowComponent>().each()) {
				if (!w.assignedSystem.empty())
					continue;
				kengine_logf(r, Log, "Init/KreoglSystem", "Found existing window: %zu", e);
				windowEntity = e;
				break;
			}

			// If none found, create one
			if (windowEntity == entt::null) {
				const auto e = r.create();
				kengine_logf(r, Log, "Init/KreoglSystem", "Created default window: %zu", e);
				r.emplace<KeepAlive>(e);
				auto & windowComp = r.emplace<WindowComponent>(e);
				windowComp.name = "Kengine";
				windowEntity = e;
			}

			// Ask the GLFW system to initialize the window
			r.get<WindowComponent>(windowEntity).assignedSystem = "Kreogl";
			r.emplace<GLFWWindowInitComponent>(windowEntity) = {
				.setHints = [this]() noexcept {
					kengine_log(r, Log, "Init/KreoglSystem", "Setting window hints");
					glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
					glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
					glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
					glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#ifndef KENGINE_NDEBUG
					glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
				},
				.onWindowCreated = [this, windowEntity]() noexcept {
					kengine_log(r, Log, "Init/KreoglSystem", "GLFW window created");
					kengine_log(r, Log, "Init/KreoglSystem", "Creating kreogl window");
					const auto & glfwWindow = r.get<GLFWWindowComponent>(windowEntity);
					auto & kreoglWindow = r.emplace<kreogl::Window>(windowEntity, *glfwWindow.window.get());
					kreoglWindow.removeCamera(kreoglWindow.getDefaultCamera());
					initImGui(windowEntity);
				}
			};
		}

		void initImGui(entt::entity windowEntity) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init/KreoglSystem", "Initializing ImGui");

			r.emplace<ImGuiContextComponent>(windowEntity, ImGui::CreateContext());

			auto & io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			io.ConfigViewportsNoTaskBarIcon = true;

			const auto & glfwComp = r.get<GLFWWindowComponent>(windowEntity);
			ImGui_ImplGlfw_InitForOpenGL(glfwComp.window.get(), true);
			ImGui_ImplOpenGL3_Init();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		void terminate() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Terminate", "KreoglSystem");

			// Clear these now, or their dtors will fail since the OpenGL state won't exist anymore
			r.clear<
			    KreoglAnimationFilesComponent,
				KreoglModelComponent,
				kreogl::AnimatedObject,
				kreogl::Camera,
				kreogl::ImageTexture,
				kreogl::DirectionalLight, kreogl::PointLight, kreogl::SpotLight,
				kreogl::SkyboxTexture,
				kreogl::Sprite2D, kreogl::Sprite3D,
				kreogl::Text2D, kreogl::Text3D,
				kreogl::Window
			>();
		}

		void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Verbose, "Execute", "KreoglSystem");

			createMissingModels();
			createMissingObjects();
			tickAnimations(deltaTime);

			updateImGuiScale();
			draw();
			endImGuiFrame();
		}

		float lastScale = 1.f;
		void updateImGuiScale() noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto scale = imguiHelper::getScale(r);
			ImGui::GetIO().FontGlobalScale = scale;
			ImGui::GetStyle().ScaleAllSizes(scale / lastScale);
			lastScale = scale;
		}

		void createMissingModels() noexcept {
			KENGINE_PROFILING_SCOPE;

			createModelsFromDisk();
			createModelsFromModelData();
		}

		void createModelsFromDisk() noexcept {
			KENGINE_PROFILING_SCOPE;

			for (auto [modelEntity, model] : r.view<ModelComponent>(entt::exclude<KreoglModelComponent, kreogl::ImageTexture>).each()) {
				kengine_logf(r, Verbose, "Execute/KreoglSystem", "Creating Kreogl model for %zu (%s)", modelEntity, model.file.c_str());
				if (kreogl::ImageTexture::isSupportedFormat(model.file.c_str()))
					r.emplace<kreogl::ImageTexture>(modelEntity, model.file.c_str());
				else if (kreogl::AssImp::isSupportedFileFormat(model.file.c_str()))
					createModelWithAssImp(modelEntity, model);
			}
		}

		void createModelWithAssImp(entt::entity modelEntity, const ModelComponent & model) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto animatedModel = kreogl::AssImp::loadAnimatedModel(model.file.c_str());
			if (animatedModel) {
				// Sync properties from loaded model to kengine components
				addAnimationsToModelAnimationComponent(r.get_or_emplace<ModelAnimationComponent>(modelEntity), model.file.c_str(), animatedModel->animations);
				if (animatedModel->skeleton) {
					auto & modelSkeleton = r.emplace<ModelSkeletonComponent>(modelEntity);
					for (const auto & mesh : animatedModel->skeleton->meshes)
						modelSkeleton.meshes.push_back(ModelSkeletonComponent::Mesh{
							.boneNames = mesh.boneNames
						});
				}
			}
			r.emplace<KreoglModelComponent>(modelEntity, std::move(animatedModel));

			// Load animations from external files
			if (const auto animationFiles = r.try_get<AnimationFilesComponent>(modelEntity)) {
				auto & modelAnimation = r.get<ModelAnimationComponent>(modelEntity);
				auto & kreoglAnimationFiles = r.emplace<KreoglAnimationFilesComponent>(modelEntity);
				for (const auto & file : animationFiles->files) {
					auto kreoglAnimationFile = kreogl::AssImp::loadAnimationFile(file.c_str());
					addAnimationsToModelAnimationComponent(modelAnimation, file.c_str(), kreoglAnimationFile->animations);
					kreoglAnimationFiles.files.push_back(std::move(kreoglAnimationFile));
				}
			}
		}

		void addAnimationsToModelAnimationComponent(ModelAnimationComponent & modelAnimation, const char * file, const std::vector<std::unique_ptr<kreogl::AnimationModel>> & animations) noexcept {
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

		void createModelsFromModelData() noexcept {
			KENGINE_PROFILING_SCOPE;

			for (auto [modelEntity, modelData] : r.view<ModelDataComponent>(entt::exclude<KreoglModelComponent>).each()) {
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
				r.emplace<KreoglModelComponent>(modelEntity, std::make_unique<kreogl::AnimatedModel>(vertexSpecification, kreoglModelData));
			}
		}

		void createMissingObjects() noexcept {
			KENGINE_PROFILING_SCOPE;

			for (auto [entity, instance] : r.view<InstanceComponent>(entt::exclude<kreogl::AnimatedObject, kreogl::Sprite2D, kreogl::Sprite3D>).each()) {
				if (instance.model == entt::null)
					continue;

				const auto modelEntity = instance.model;

				if (const auto kreoglModel = r.try_get<KreoglModelComponent>(modelEntity)) {
					r.emplace<kreogl::AnimatedObject>(entity).model = kreoglModel->model.get();
					if (r.all_of<ModelSkeletonComponent>(modelEntity))
						r.emplace<SkeletonComponent>(entity);
				}

				if (const auto kreoglTexture = r.try_get<kreogl::ImageTexture>(modelEntity)) {
					if (r.all_of<SpriteComponent2D>(entity))
						r.emplace<kreogl::Sprite2D>(entity).texture = kreoglTexture;
					if (r.all_of<SpriteComponent3D>(entity))
						r.emplace<kreogl::Sprite3D>(entity).texture = kreoglTexture;
				}
			}

			for (auto [text2DEntity, text2D] : r.view<TextComponent2D>(entt::exclude<kreogl::Text2D>).each())
				r.emplace<kreogl::Text2D>(text2DEntity);

			for (auto [text3DEntity, text3D] : r.view<TextComponent3D>(entt::exclude<kreogl::Text3D>).each())
				r.emplace<kreogl::Text3D>(text3DEntity);

			for (auto [lightEntity, dirLight] : r.view<DirLightComponent>(entt::exclude<kreogl::DirectionalLight>).each())
				r.emplace<kreogl::DirectionalLight>(lightEntity);

			for (auto [lightEntity, dirLight] : r.view<PointLightComponent>(entt::exclude<kreogl::PointLight>).each())
				r.emplace<kreogl::PointLight>(lightEntity);

			for (auto [lightEntity, dirLight] : r.view<SpotLightComponent>(entt::exclude<kreogl::SpotLight>).each())
				r.emplace<kreogl::SpotLight>(lightEntity);

			for (auto [skyboxEntity, skyBox] : r.view<SkyBoxComponent>(entt::exclude<kreogl::SkyboxTexture>).each())
				r.emplace<kreogl::SkyboxTexture>(skyboxEntity, skyBox.left.c_str(), skyBox.right.c_str(), skyBox.top.c_str(), skyBox.bottom.c_str(), skyBox.front.c_str(), skyBox.back.c_str());

			for (auto [debugEntity, debugGraphics] : r.view<DebugGraphicsComponent>(entt::exclude<KreoglDebugGraphicsComponent>).each())
				r.emplace<KreoglDebugGraphicsComponent>(debugEntity);
		}

		void tickAnimations(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto view = r.view<kreogl::AnimatedObject, AnimationComponent>();
			std::for_each(std::execution::par_unseq, view.begin(), view.end(), [&](entt::entity entity) noexcept {
				const auto & [kreoglObject, animation] = view.get(entity);
				tickObjectAnimation(deltaTime, entity, kreoglObject, animation);
			});
		}

		void tickObjectAnimation(float deltaTime, entt::entity entity, kreogl::AnimatedObject & kreoglObject, AnimationComponent & animation) noexcept {
			KENGINE_PROFILING_SCOPE;
			if (!kreoglObject.animation)
				return;

			kreoglObject.tickAnimation(deltaTime);

			// Sync properties from kreoglObject to kengine components
			animation.currentTime = kreoglObject.animation->currentTime;

			auto & skeleton = r.get<SkeletonComponent>(entity);
			const auto nbMeshes = kreoglObject.skeleton.meshes.size();
			skeleton.meshes.resize(nbMeshes);
			for (size_t i = 0; i < nbMeshes; ++i) {
				const auto & kreoglMesh = kreoglObject.skeleton.meshes[i];
				auto & mesh = skeleton.meshes[i];

				kengine_assert(r, kreoglMesh.boneMatsBoneSpace.size() < putils::lengthof(mesh.boneMatsBoneSpace));
				std::ranges::copy(kreoglMesh.boneMatsBoneSpace, mesh.boneMatsBoneSpace);
				kengine_assert(r, kreoglMesh.boneMatsMeshSpace.size() < putils::lengthof(mesh.boneMatsMeshSpace));
				std::ranges::copy(kreoglMesh.boneMatsMeshSpace, mesh.boneMatsMeshSpace);
			}
		}

		void draw() noexcept {
			KENGINE_PROFILING_SCOPE;

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			for (const auto & [windowEntity, kreoglWindow] : r.view<kreogl::Window>().each()) {
				kreoglWindow.prepareForDraw();
				drawToCameras(windowEntity, kreoglWindow);
				kreoglWindow.display();
			}
		}

		void drawToCameras(entt::entity windowEntity, kreogl::Window & kreoglWindow) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (auto [cameraEntity, camera, viewport] : r.view<CameraComponent, ViewportComponent>().each()) {
				if (viewport.window != entt::null && viewport.window != windowEntity)
					continue;

				if (viewport.window == entt::null) {
					kengine_logf(r, Log, "KreoglSystem", "Setting target window for ViewportComponent in %zu", cameraEntity);
					viewport.window = windowEntity;
					createKreoglCamera(cameraEntity, viewport);
				}

				auto & kreoglCamera = r.get<kreogl::Camera>(cameraEntity);
				syncCameraProperties(kreoglCamera, camera, viewport);
				drawToCamera(kreoglWindow, cameraEntity, kreoglCamera);
			}
		}

		void createKreoglCamera(entt::entity cameraEntity, ViewportComponent & viewport) noexcept {
			KENGINE_PROFILING_SCOPE;

			const kreogl::Camera::ConstructionParams params{
				.viewport = {
					.resolution = toglm(viewport.resolution)
				}
			};
			auto & kreoglCamera = r.emplace<kreogl::Camera>(cameraEntity, params);
			viewport.renderTexture = ViewportComponent::RenderTexture(kreoglCamera.getViewport().getRenderTexture());
		}

		void syncCameraProperties(kreogl::Camera & kreoglCamera, const CameraComponent & camera, const ViewportComponent & viewport) noexcept {
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

		// Lazy-init these as OpenGL context doesn't exist upon construction, so can't init the shader
		std::optional<HighlightShader> highlightShader;
		std::optional<kreogl::ShaderPipeline> shaderPipeline;

		void drawToCamera(kreogl::Window & kreoglWindow, entt::entity cameraEntity, const kreogl::Camera & kreoglCamera) noexcept {
			KENGINE_PROFILING_SCOPE;

			kreogl::World kreoglWorld;
			syncEverything(kreoglWorld, cameraEntity);

			if (!shaderPipeline) {
				highlightShader = HighlightShader{ r };
				shaderPipeline = [this] {
					auto ret = kreogl::ShaderPipeline::getDefaultShaders();
					ret.addShader(kreogl::ShaderStep::PostProcess, *highlightShader);
					return ret;
				}();
			}

			kreoglWindow.drawWorldToCamera(kreoglWorld, kreoglCamera, *shaderPipeline);
		}


		void syncEverything(kreogl::World & kreoglWorld, entt::entity cameraEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			syncAllObjects(kreoglWorld, cameraEntity);
			syncAllLights(kreoglWorld, cameraEntity);
		}

		void syncCommonProperties(auto & kreoglObject, entt::entity entity, const InstanceComponent * instance, const TransformComponent & transform, const auto & coloredComponent, kreogl::World & kreoglWorld) noexcept {
			KENGINE_PROFILING_SCOPE;

			const TransformComponent * modelTransform = nullptr;
			if (instance)
				modelTransform = instanceHelper::tryGetModel<TransformComponent>(r, *instance);
			kreoglObject.transform = matrixHelper::getModelMatrix(transform, modelTransform);
			kreoglObject.color = toglm(coloredComponent.color);
			kreoglObject.userData[0] = float(entity);

			kreoglWorld.add(kreoglObject);
		};

		void syncAllObjects(kreogl::World & kreoglWorld, entt::entity cameraEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [entity, instance, transform, graphics, kreoglObject] : r.view<InstanceComponent, TransformComponent, GraphicsComponent, kreogl::AnimatedObject>().each()) {
				if (!cameraHelper::entityAppearsInViewport(r, entity, cameraEntity))
					continue;
				syncCommonProperties(kreoglObject, entity, &instance, transform, graphics, kreoglWorld);
				syncAnimationProperties(kreoglObject, entity, instance);
				kreoglObject.castShadows = !r.all_of<NoShadowComponent>(entity);
			}

			syncSprite2DProperties(kreoglWorld, cameraEntity);
			for (const auto & [spriteEntity, instance, transform, graphics, kreoglSprite3D] : r.view<InstanceComponent, TransformComponent, GraphicsComponent, SpriteComponent3D, kreogl::Sprite3D>().each()) {
				if (!cameraHelper::entityAppearsInViewport(r, spriteEntity, cameraEntity))
					continue;
				syncCommonProperties(kreoglSprite3D, spriteEntity, &instance, transform, graphics, kreoglWorld);
			}

			syncText2DProperties(kreoglWorld, cameraEntity);
			for (const auto & [textEntity, transform, text3D, kreoglText3D] : r.view<TransformComponent, TextComponent3D, kreogl::Text3D>().each()) {
				if (!cameraHelper::entityAppearsInViewport(r, textEntity, cameraEntity))
					continue;
				syncTextProperties(kreoglText3D, textEntity, transform, text3D, kreoglWorld);
			}

			syncDebugGraphicsProperties(kreoglWorld, cameraEntity);

			for (const auto & [skyboxEntity, skyBox, kreoglSkybox] : r.view<SkyBoxComponent, kreogl::SkyboxTexture>().each()) {
				if (!cameraHelper::entityAppearsInViewport(r, skyboxEntity, cameraEntity))
					continue;
				kreoglWorld.skybox.color = toglm(skyBox.color);
				kreoglWorld.skybox.texture = &kreoglSkybox;
			}
		}

		void syncAnimationProperties(kreogl::AnimatedObject & kreoglObject, entt::entity entity, const InstanceComponent & instance) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto animation = r.try_get<AnimationComponent>(entity);
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

		const kreogl::AnimationModel * findAnimationModel(const InstanceComponent & instance, size_t animationIndex) {
			KENGINE_PROFILING_SCOPE;

			const auto & kreoglModel = instanceHelper::getModel<KreoglModelComponent>(r, instance);
			if (animationIndex < kreoglModel.model->animations.size())
				return kreoglModel.model->animations[animationIndex].get();

			const auto kreoglModelAnimationFiles = instanceHelper::tryGetModel<KreoglAnimationFilesComponent>(r, instance);
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

		void syncSprite2DProperties(kreogl::World & kreoglWorld, entt::entity cameraEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [spriteEntity, instance, transform, graphics, sprite2D, kreoglSprite2D] : r.view<InstanceComponent, TransformComponent, GraphicsComponent, SpriteComponent2D, kreogl::Sprite2D>().each()) {
				if (!cameraHelper::entityAppearsInViewport(r, spriteEntity, cameraEntity))
					continue;

				syncCommonProperties(kreoglSprite2D, spriteEntity, &instance, transform, graphics, kreoglWorld);

				const auto & viewport = r.get<ViewportComponent>(cameraEntity);
				const auto & box = cameraHelper::convertToScreenPercentage(transform.boundingBox, viewport.resolution, sprite2D);
				kreoglSprite2D.transform = getOnScreenMatrix(transform, box.position, box.size, sprite2D);
			}
		}

		void syncText2DProperties(kreogl::World & kreoglWorld, entt::entity cameraEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [textEntity, transform, text2D, kreoglText2D] : r.view<TransformComponent, TextComponent2D, kreogl::Text2D>().each()) {
				if (!cameraHelper::entityAppearsInViewport(r, textEntity, cameraEntity))
					continue;

				syncTextProperties(kreoglText2D, textEntity, transform, text2D, kreoglWorld);

				const auto & viewport = r.get<ViewportComponent>(cameraEntity);
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

		void syncDebugGraphicsProperties(kreogl::World & kreoglWorld, entt::entity cameraEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [debugEntity, transform, debugGraphics, kreoglDebugGraphics] : r.view<TransformComponent, DebugGraphicsComponent, KreoglDebugGraphicsComponent>().each()) {
				if (!cameraHelper::entityAppearsInViewport(r, debugEntity, cameraEntity))
					continue;

				kreoglDebugGraphics.elements.resize(debugGraphics.elements.size());
				for (size_t i = 0; i < debugGraphics.elements.size(); ++i) {
					const auto & element = debugGraphics.elements[i];
					auto & kreoglElement = kreoglDebugGraphics.elements[i];
					kreoglElement.color = toglm(element.color);
					kreoglElement.userData[0] = float(debugEntity);

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
							kengine_assert_failed(r, "Non-exhaustive switch");
							break;
					}

					kreoglWorld.add(kreoglElement);
				}
			}
		}

		void applyDebugGraphicsTransform(glm::mat4 & matrix, const TransformComponent & transform, const glm::vec3 & pos, const glm::vec3 & size, DebugGraphicsComponent::ReferenceSpace referenceSpace) noexcept {
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

		glm::mat4 getOnScreenMatrix(const TransformComponent & transform, const putils::Point3f & position, const putils::Point3f & size, const OnScreenComponent & onScreen) noexcept {
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

		void syncTextProperties(auto & kreoglText, entt::entity textEntity, const TransformComponent & transform, const TextComponent & text, kreogl::World & kreoglWorld) noexcept {
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
					kengine_assert_failed(r, "Non-exhaustive switch");
					break;
			}
		}

		void syncAllLights(kreogl::World & kreoglWorld, entt::entity cameraEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			syncAllDirLights(kreoglWorld, cameraEntity);
			syncAllPointLights(kreoglWorld, cameraEntity);
			syncAllSpotLights(kreoglWorld, cameraEntity);
		}

		void syncAllDirLights(kreogl::World & kreoglWorld, entt::entity cameraEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [lightEntity, dirLight, kreoglDirLight] : r.view<DirLightComponent, kreogl::DirectionalLight>().each()) {
				if (!cameraHelper::entityAppearsInViewport(r, lightEntity, cameraEntity))
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

		void syncAllPointLights(kreogl::World & kreoglWorld, entt::entity cameraEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [lightEntity, transform, pointLight, kreoglPointLight] : r.view<TransformComponent, PointLightComponent, kreogl::PointLight>().each()) {
				if (!cameraHelper::entityAppearsInViewport(r, lightEntity, cameraEntity))
					continue;
				syncPointLightProperties(lightEntity, transform, kreoglPointLight, pointLight, kreoglWorld);
			}
		}

		void syncAllSpotLights(kreogl::World & kreoglWorld, entt::entity cameraEntity) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [lightEntity, transform, spotLight, kreoglSpotLight] : r.view<TransformComponent, SpotLightComponent, kreogl::SpotLight>().each()) {
				if (!cameraHelper::entityAppearsInViewport(r, lightEntity, cameraEntity))
					continue;

				syncPointLightProperties(lightEntity, transform, kreoglSpotLight, spotLight, kreoglWorld);

				kreoglSpotLight.direction = toglm(spotLight.direction);
				kreoglSpotLight.cutOff = spotLight.cutOff;
				kreoglSpotLight.outerCutOff = spotLight.outerCutOff;
			}
		}

		void syncLightProperties(entt::entity lightEntity, auto & kreoglLight, const LightComponent & light, kreogl::World & kreoglWorld) noexcept {
			KENGINE_PROFILING_SCOPE;

			kreoglLight.color = toglm(light.color);
			kreoglLight.diffuseStrength = light.diffuseStrength;
			kreoglLight.specularStrength = light.specularStrength;
			kreoglLight.castShadows = light.castShadows;
			kreoglLight.shadowPCFSamples = light.shadowPCFSamples;
			kreoglLight.shadowMapSize = light.shadowMapSize;
			kreoglLight.shadowMapMaxBias = light.shadowMapMaxBias;
			kreoglLight.shadowMapMinBias = light.shadowMapMinBias;

			if (const auto godRays = r.try_get<GodRaysComponent>(lightEntity)) {
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

		void syncPointLightProperties(entt::entity lightEntity, const TransformComponent & transform, kreogl::PointLight & kreoglLight, const PointLightComponent & light, kreogl::World & kreoglWorld) noexcept {
			KENGINE_PROFILING_SCOPE;

			syncLightProperties(lightEntity, kreoglLight, light, kreoglWorld);

			kreoglLight.position = toglm(transform.boundingBox.position);
			kreoglLight.range = light.range;
			kreoglLight.attenuationConstant = light.attenuationConstant;
			kreoglLight.attenuationLinear = light.attenuationLinear;
			kreoglLight.attenuationQuadratic = light.attenuationQuadratic;
		}

		void endImGuiFrame() noexcept {
			KENGINE_PROFILING_SCOPE;

			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				GLFWwindow * backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			if (!r.view<GLFWWindowComponent>().empty())
				ImGui::NewFrame();
		}

		entt::entity getEntityInPixel(entt::entity windowID, const putils::Point2ui & pixel) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, Verbose, "KreoglSystem", "Getting entity in { %zu, %zu } of %zu", pixel.x, pixel.y, windowID);

			const auto entityIDInPixel = readFromGBuffer(windowID, pixel, kreogl::GBuffer::Texture::UserData);
			if (!entityIDInPixel)
				return entt::null;

			const auto ret = entt::entity(entityIDInPixel->r);
			if (ret == entt::entity(entt::id_type(0))) {
				kengine_log(r, Verbose, "OpenGLSystem/getEntityInPixel", "Found no Entity");
				return entt::null;
			}

			kengine_logf(r, Verbose, "OpenGLSystem/getEntityInPixel", "Found %zu", ret);
			return ret;
		}

		std::optional<putils::Point3f> getPositionInPixel(entt::entity window, const putils::Point2ui & pixel) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, Verbose, "KreoglSystem", "Getting position in { %zu, %zu } of %zu", pixel.x, pixel.y, window);

			// Check that an entity was drawn in the pixel, otherwise position buffer is invalid
			if (getEntityInPixel(window, pixel) == entt::null)
				return std::nullopt;

			const auto positionInPixel = readFromGBuffer(window, pixel, kreogl::GBuffer::Texture::Position);
			if (!positionInPixel)
				return std::nullopt;

			kengine_logf(r, Verbose, "KreoglSystem/getPositionInPixel", "Found { %f, %f, %f }", positionInPixel->x, positionInPixel->y, positionInPixel->y);
			return &positionInPixel->x;
		}

		std::optional<glm::vec4> readFromGBuffer(entt::entity window, const putils::Point2ui & pixel, kreogl::GBuffer::Texture texture) noexcept {
			KENGINE_PROFILING_SCOPE;

			static constexpr auto GBUFFER_TEXTURE_COMPONENTS = 4;

			if (window == entt::null) {
				for (const auto & [windowEntity, kreoglWindow] : r.view<kreogl::Window>().each()) {
					window = windowEntity;
					break;
				}

				if (window == entt::null) {
					kengine_log(r, Warning, "KreoglSystem", "No existing Kreogl window");
					return std::nullopt;
				}
			}

			if (!r.all_of<kreogl::Window>(window)) {
				kengine_logf(r, Warning, "KreoglSystem", "%zu does not have a Kreogl window", window);
				return std::nullopt;
			}

			const auto viewportInfo = cameraHelper::getViewportForPixel({ r, window }, pixel);
			if (viewportInfo.camera == entt::null) {
				kengine_logf(r, Warning, "KreoglSystem", "Found no viewport containing pixel { %d, %d }", pixel.x, pixel.y);
				return std::nullopt;
			}

			const auto cameraEntity = viewportInfo.camera;
			const auto kreoglCamera = r.try_get<kreogl::Camera>(cameraEntity);
			if (!kreoglCamera) {
				kengine_logf(r, Warning, "KreoglSystem", "Viewport %zu does not have a kreogl::Camera", cameraEntity);
				return std::nullopt;
			}

			const auto & viewport = kreoglCamera->getViewport();
			const auto & gBuffer = viewport.getGBuffer();
			const auto gBufferSize = viewport.getResolution();

			const auto pixelInGBuffer = glm::vec2(gBufferSize) * toglm(viewportInfo.viewportPercent);
			if (pixelInGBuffer.x >= float(gBufferSize.x) || pixelInGBuffer.y > float(gBufferSize.y) || pixelInGBuffer.y == 0) {
				kengine_logf(r, Warning, "OpenGLSystem/getEntityInPixel", "Pixel is out of %zu's GBuffer's bounds", cameraEntity);
				return std::nullopt;
			}

			return gBuffer.readPixel(texture, pixelInGBuffer);
		}
	};

	void addKreoglSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<KreoglSystem>(e);
	}
}