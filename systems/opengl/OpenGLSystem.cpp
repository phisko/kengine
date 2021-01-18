#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "OpenGLSystem.hpp"
#include "Controllers.hpp"

#include "kengine.hpp"

#include "opengl/Program.hpp"
#include "opengl/RAII.hpp"
#include "opengl/Mesh.hpp"

#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include "data/ModelDataComponent.hpp"
#include "data/SystemSpecificTextureComponent.hpp"
#include "data/TextureDataComponent.hpp"
#include "data/ModelComponent.hpp"
#include "data/SystemSpecificModelComponent.hpp"
#include "data/ImGuiContextComponent.hpp"
#include "data/ImGuiScaleComponent.hpp"
#include "data/AdjustableComponent.hpp"
#include "data/CameraComponent.hpp"
#include "data/ViewportComponent.hpp"
#include "data/WindowComponent.hpp"
#include "data/GLFWWindowComponent.hpp"
#include "data/ShaderComponent.hpp"
#include "data/GBufferComponent.hpp"

#include "functions/Execute.hpp"
#include "functions/OnTerminate.hpp"
#include "functions/OnEntityCreated.hpp"
#include "functions/OnEntityRemoved.hpp"
#include "functions/OnMouseCaptured.hpp"
#include "functions/GetEntityInPixel.hpp"
#include "functions/GetPositionInPixel.hpp"
#include "functions/InitGBuffer.hpp"

#include "shaders/shaderHelper.hpp"
#include "helpers/cameraHelper.hpp"
#include "helpers/matrixHelper.hpp"
#include "helpers/assertHelper.hpp"
#include "helpers/imguiHelper.hpp"

#include "shaders/ShadowMap.hpp"
#include "shaders/ShadowCube.hpp"
#include "shaders/SpotLight.hpp"
#include "shaders/DirLight.hpp"
#include "shaders/PointLight.hpp"
#include "shaders/LightSphere.hpp"
#include "shaders/GodRaysDirLight.hpp"
#include "shaders/GodRaysPointLight.hpp"
#include "shaders/GodRaysSpotLight.hpp"
#include "shaders/Highlight.hpp"
#include "shaders/Debug.hpp"
#include "shaders/SkyBox.hpp"
#include "shaders/Text.hpp"

#include "Timer.hpp"
#include "concat.hpp"

#ifndef KENGINE_MAX_VIEWPORTS
# define KENGINE_MAX_VIEWPORTS 8
#endif

namespace kengine::opengl {
	using MyShaderComponent = SystemSpecificShaderComponent<putils::gl::Program>;
	using MyModelComponent = SystemSpecificModelComponent<putils::gl::Mesh>;
	using MyTextureComponent = SystemSpecificTextureComponent<putils::gl::Texture>;

	struct impl {
		static inline putils::gl::Program::Parameters params;

		static inline size_t gBufferTextureCount = 0;
		static inline functions::GBufferAttributeIterator gBufferIterator = nullptr;

		static inline struct {
			EntityID id = INVALID_ID;
			GLFWWindowComponent * glfw = nullptr;
			WindowComponent * comp = nullptr;
		} window;

		static void init(Entity & e) noexcept {
			e += functions::Execute{ execute };
			e += functions::OnEntityCreated{ onEntityCreated };
			e += functions::OnEntityRemoved{ onEntityRemoved };
			e += functions::OnTerminate{ terminate };
			e += functions::OnMouseCaptured{ onMouseCaptured };
			e += functions::GetEntityInPixel{ getEntityInPixel };
			e += functions::GetPositionInPixel{ getPositionInPixel };

			auto & scale = e.attach<ImGuiScaleComponent>();

			e += AdjustableComponent{
				"ImGui", {
					{ "Scale", &scale.scale }
				}
			};

			e += functions::InitGBuffer{ initGBuffer };

#if !defined(KENGINE_NDEBUG) && !defined(KENGINE_OPENGL_NO_DEBUG_TOOLS)
			entities += opengl::ShaderController();
			entities += opengl::GBufferDebugger(gBufferIterator);
#endif

			params.nearPlane = 1.f;
			params.farPlane = 1000.f;

			init();
		}

		static void init() noexcept {
			if (window.comp == nullptr) {
				for (const auto & [e, w] : entities.with<WindowComponent>()) {
					if (!w.assignedSystem.empty())
						continue;
					window.id = e.id;
					window.comp = &w;
					break;
				}

				if (window.id == INVALID_ID) {
					entities += [](Entity & e) {
						window.comp = &e.attach<WindowComponent>();
						window.comp->name = "Kengine";
						window.comp->size = { 1280, 720 };
						window.id = e.id;
					};
				}
			}

			window.comp->assignedSystem = "OpenGL";

			auto e = entities[window.id];
			e += GLFWWindowInitComponent{
				.setHints = []() noexcept {
					glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
					glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
					glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
					glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
			#ifndef KENGINE_NDEBUG
					glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
			#endif
				},
				.onWindowCreated = []() noexcept {
					window.glfw = &entities[window.id].get<GLFWWindowComponent>();
					glewExperimental = true;
					const bool ret = glewInit();
					kengine_assert(ret == GLEW_OK);

					initImGui();

			#ifndef KENGINE_NDEBUG
					glEnable(GL_DEBUG_OUTPUT);
					glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
					glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void * userParam) {
						if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
							std::cerr <<
							putils::termcolor::red <<
							"G: severity = 0x" << std::ios::hex << severity << std::ios::dec <<
							", message: " << message << '\n' <<
							putils::termcolor::reset;
					}, nullptr);
			#endif

			#ifndef KENGINE_OPENGL_NO_DEFAULT_SHADERS
					addShaders();
			#endif
				}
			};
		}

		static void initImGui() noexcept {
			entities[window.id] += ImGuiContextComponent{
				ImGui::CreateContext()
			};

			auto & io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			io.ConfigViewportsNoTaskBarIcon = true;

			ImGui_ImplGlfw_InitForOpenGL(window.glfw->window.get(), true);
			ImGui_ImplOpenGL3_Init();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		static void addShaders() noexcept {
			{ // GBuffer
				entities += [=](Entity & e) noexcept {
					e += MyShaderComponent{ std::make_unique<shaders::Debug>() };
					e += GBufferShaderComponent{};
				};
				entities += [=](Entity & e) noexcept {
					e += MyShaderComponent{ std::make_unique<shaders::Text>() };
					e += GBufferShaderComponent{};
				};
			}

			{ // Lighting
				entities += [&](Entity & e) noexcept {
					e += MyShaderComponent{ std::make_unique<shaders::ShadowMap>(e) };
					e += ShadowMapShaderComponent{};
				};

				entities += [&](Entity & e) noexcept {
					e += MyShaderComponent{ std::make_unique<shaders::ShadowCube>() };
					e += ShadowCubeShaderComponent{};
				};

				entities += [=](Entity & e) noexcept {
					e += MyShaderComponent{ std::make_unique<shaders::SpotLight>() };
					e += LightingShaderComponent{};
				};
				entities += [=](Entity & e) noexcept {
					e += MyShaderComponent{ std::make_unique<shaders::DirLight>(e) };
					e += LightingShaderComponent{};
				};
				entities += [=](Entity & e) noexcept {
					e += MyShaderComponent{ std::make_unique<shaders::PointLight>() };
					e += LightingShaderComponent{};
				};
			}

			{ // Post lighting
				entities += [=](Entity & e) noexcept {
					e += MyShaderComponent{ std::make_unique<shaders::GodRaysDirLight>() };
					e += PostLightingShaderComponent{};
				};
				entities += [=](Entity & e) noexcept {
					e += MyShaderComponent{ std::make_unique<shaders::GodRaysPointLight>() };
					e += PostLightingShaderComponent{};
				};
				entities += [=](Entity & e) noexcept {
					e += MyShaderComponent{ std::make_unique<shaders::GodRaysSpotLight>() };
					e += PostLightingShaderComponent{};
				};
			}

			{ // Post process
				entities += [=](Entity & e) noexcept {
					e += MyShaderComponent{ std::make_unique<shaders::LightSphere>(e) };
					e += PostProcessShaderComponent{};
				};
				entities += [=](Entity & e) noexcept {
					e += MyShaderComponent{ std::make_unique<shaders::Highlight>() };
					e += PostProcessShaderComponent{};
				};
				entities += [=](Entity & e) noexcept {
					e += MyShaderComponent{ std::make_unique<shaders::SkyBox>() };
					e += PostProcessShaderComponent{};
				};
			}
		}

		static void onEntityCreated(Entity & e) noexcept {
			if (gBufferIterator == nullptr)
				return;

			const auto shader = e.tryGet<MyShaderComponent>();
			if (shader)
				initShader(*shader->shader);
		}

		static void initShader(putils::gl::Program & p) noexcept {
			p.init(gBufferTextureCount);

			kengine_assert(gBufferIterator != nullptr);
			int texture = 0;
			gBufferIterator([&](const char * name) noexcept {
				p.addGBufferTexture(name, texture++);
			});
		}

		static void onEntityRemoved(Entity & e) noexcept {
			const auto viewport = e.tryGet<ViewportComponent>();
			if (viewport) {
				if (viewport->window == window.id) {
					GLuint texture = (GLuint)viewport->renderTexture;
					glDeleteTextures(1, &texture);
				}
			}

			if (e.id != window.id)
				return;
			window.id = INVALID_ID;
			window.glfw = nullptr;
			window.comp = nullptr;
			terminate();
		}

		static void onMouseCaptured(EntityID w, bool captured) noexcept {
			if (w != INVALID_ID && w != window.id)
				return;

			if (captured) {
				glfwSetInputMode(window.glfw->window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
			}
			else {
				glfwSetInputMode(window.glfw->window.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
			}
		}

		static void execute(float deltaTime) noexcept {
			if (window.id == INVALID_ID || window.glfw == nullptr)
				return;

			for (auto [e, modelData, noOpenGL] : entities.with<ModelDataComponent, no<MyModelComponent>>())
				createObject(e, modelData);

			for (auto [e, textureData, noTextureModel] : entities.with<TextureDataComponent, no<MyTextureComponent>>())
				loadTexture(e, textureData);

			doOpenGL();

			static float lastScale = 1.f;
			const auto scale = imguiHelper::getScale();
			ImGui::GetIO().FontGlobalScale = scale;
			ImGui::GetStyle().ScaleAllSizes(scale / lastScale);
			lastScale = scale;

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
				GLFWwindow * backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}

			glfwSwapBuffers(window.glfw->window.get());

			glfwPollEvents();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		static void createObject(Entity & e, const ModelDataComponent & modelData) noexcept {
			auto & openglModel = e.attach<MyModelComponent>();

			for (const auto & meshData : modelData.meshes) {
				putils::gl::Mesh openglMesh;
				openglMesh.vertexArrayObject.generate();
				glBindVertexArray(openglMesh.vertexArrayObject);

				openglMesh.vertexBuffer.generate();
				glBindBuffer(GL_ARRAY_BUFFER, openglMesh.vertexBuffer);
				glBufferData(GL_ARRAY_BUFFER, meshData.vertices.nbElements * meshData.vertices.elementSize, meshData.vertices.data, GL_STATIC_DRAW);

				openglMesh.indexBuffer.generate();
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, openglMesh.indexBuffer);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData.indices.nbElements * meshData.indices.elementSize, meshData.indices.data, GL_STATIC_DRAW);

				size_t location = 0;
				for (const auto & attribute : modelData.vertexAttributes)
					registerVertexAttribute(modelData.vertexSize, location++, attribute.offset, attribute.type);

				openglMesh.nbIndices = meshData.indices.nbElements;

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
				const auto it = types.find(meshData.indexType);
				if (it == types.end())
					kengine_assert_failed("Unknown index type");
				else
					openglMesh.indexType = it->second;

				openglModel.meshes.push_back(std::move(openglMesh));
			}
		}

		static void registerVertexAttribute(size_t vertexSize, size_t location, size_t offset, putils::meta::type_index type) noexcept {
			struct VertexType {
				GLenum type;
				size_t length;
			};

#define VERTEX_TYPE(realtype, glenum, length) \
			{ putils::meta::type<realtype[length]>::index, { glenum, length }}

#define VERTEX_TYPE_FAMILY(realtype, glenum) \
			{ putils::meta::type<realtype>::index, { glenum, 1 } }, \
			VERTEX_TYPE(realtype, glenum, 1), \
			VERTEX_TYPE(realtype, glenum, 2), \
			VERTEX_TYPE(realtype, glenum, 3), \
			VERTEX_TYPE(realtype, glenum, 4)

			static const std::unordered_map<putils::meta::type_index, VertexType> types = {
				VERTEX_TYPE_FAMILY(char, GL_BYTE),
				VERTEX_TYPE_FAMILY(unsigned char, GL_UNSIGNED_BYTE),
				VERTEX_TYPE_FAMILY(short, GL_SHORT),
				VERTEX_TYPE_FAMILY(unsigned short, GL_UNSIGNED_SHORT),
				VERTEX_TYPE_FAMILY(float, GL_FLOAT),
				VERTEX_TYPE_FAMILY(int, GL_INT),
				VERTEX_TYPE_FAMILY(unsigned int, GL_UNSIGNED_INT)
			};

			const auto it = types.find(type);
			if (it == types.end()) {
				kengine_assert_failed("Unknown vertex attribute type");
				return;
			}

			glEnableVertexAttribArray((GLuint)location);
			if (it->second.type == GL_FLOAT)
				glVertexAttribPointer((GLuint)location, (GLint)it->second.length, it->second.type, GL_FALSE, (GLsizei)vertexSize, (void *)offset);
			else
				glVertexAttribIPointer((GLuint)location, (GLint)it->second.length, it->second.type, (GLsizei)vertexSize, (void *)offset);
		}

		static void loadTexture(Entity & e, const TextureDataComponent & textureData) noexcept {
			auto & textureModel = e.attach<MyTextureComponent>();

			glGenTextures(1, &textureModel.texture.get());

			if (textureData.data != nullptr) {
				GLenum format;

				switch (textureData.components) {
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
					kengine_assert_failed(putils::concat("Incompatible number of texture components: ", textureData.components));
				}

				glBindTexture(GL_TEXTURE_2D, textureModel.texture);
				glTexImage2D(GL_TEXTURE_2D, 0, format, textureData.width, textureData.height, 0, format, GL_UNSIGNED_BYTE, textureData.data);
				glGenerateMipmap(GL_TEXTURE_2D);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
		}

		struct CameraFramebufferComponent {
			putils::gl::FrameBuffer fbo;
			putils::gl::Texture depthTexture;
			putils::Point2i resolution;
		};

		static void doOpenGL() noexcept {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			struct ToBlit {
				const CameraFramebufferComponent * fb;
				const ViewportComponent * viewport;
			};
			putils::vector<ToBlit, KENGINE_MAX_VIEWPORTS> toBlit;

			for (auto [e, cam, viewport] : entities.with<CameraComponent, ViewportComponent>()) {
				if (viewport.window == INVALID_ID)
					viewport.window = window.id;
				else if (viewport.window != window.id)
					continue;

				params.viewportID = e.id;
				setupParams(cam, viewport);
				fillGBuffer(e, viewport);

				auto fb = e.tryGet<CameraFramebufferComponent>();
				if (!fb || fb->resolution != viewport.resolution) {
					fb = initFramebuffer(e);
					if (!fb)
						continue;
				}

				renderToTexture(*fb);
				if (viewport.boundingBox.size.x > 0 && viewport.boundingBox.size.y > 0)
					toBlit.push_back(ToBlit{ fb, &viewport });
			}

			std::sort(toBlit.begin(), toBlit.end(), [](const ToBlit & lhs, const ToBlit & rhs) noexcept {
				return lhs.viewport->zOrder < rhs.viewport->zOrder;
				});

			for (const auto & blit : toBlit)
				blitTextureToViewport(*blit.fb, *blit.viewport);
		}

		static void setupParams(const CameraComponent & cam, const ViewportComponent & viewport) noexcept {
			params.viewport.size = viewport.resolution;
			putils::gl::setViewPort(params.viewport);

			params.camPos = shaderHelper::toVec(cam.frustum.position);
			params.camFOV = cam.frustum.size.y;

			params.proj = matrixHelper::getProjMatrix(cam, viewport, params.nearPlane, params.farPlane);
			params.view = matrixHelper::getViewMatrix(cam, viewport);
		}

		static CameraFramebufferComponent * initFramebuffer(Entity & e) noexcept {
			auto & viewport = e.get<ViewportComponent>();
			if (viewport.resolution.x == 0 || viewport.resolution.y == 0)
				return nullptr;

			auto & fb = e.attach<CameraFramebufferComponent>();
			fb.resolution = viewport.resolution;

			fb.fbo.generate();
			glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);

			GLuint texture = (GLuint)viewport.renderTexture;
			if (viewport.renderTexture == (ViewportComponent::RenderTexture) - 1)
				glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, viewport.resolution.x, viewport.resolution.y, 0, GL_RGBA, GL_FLOAT, nullptr);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

			fb.depthTexture.generate();
			glBindTexture(GL_TEXTURE_2D, fb.depthTexture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, viewport.resolution.x, viewport.resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fb.depthTexture, 0);

			kengine_assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

			viewport.renderTexture = (ViewportComponent::RenderTexture)texture;

			return &fb;
		}

		template<typename Tag>
		static void runShaders() noexcept {
			for (auto [e, comp, tag] : entities.with<MyShaderComponent, Tag>()) {
				if (!cameraHelper::entityAppearsInViewport(e, params.viewportID))
					continue;
				if (!comp.enabled)
					continue;

#ifndef KENGINE_NDEBUG
				struct ShaderProfiler {
					ShaderProfiler(Entity & e) noexcept {
						_comp = &e.attach<opengl::ShaderProfileComponent>();
						_timer.restart();
					}

					~ShaderProfiler() noexcept {
						_comp->executionTime = _timer.getTimeSinceStart().count();
					}

					opengl::ShaderProfileComponent * _comp;
					putils::Timer _timer;
				};
				ShaderProfiler _(e);
#endif
				comp.shader->run(params);
			}
		}

		static void fillGBuffer(Entity & e, const ViewportComponent & viewport) noexcept {
			auto gbuffer = e.tryGet<GBufferComponent>();
			if (!gbuffer) {
				gbuffer = &e.attach<GBufferComponent>();
				gbuffer->init(viewport.resolution.x, viewport.resolution.y, gBufferTextureCount);
			}
			if (gbuffer->getSize() != viewport.resolution)
				gbuffer->resize(viewport.resolution.x, viewport.resolution.y);

			gbuffer->bindForWriting();
			{
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				shaderHelper::Enable depth(GL_DEPTH_TEST);
				runShaders<GBufferShaderComponent>();
			}
			gbuffer->bindForReading();
		}

		static void renderToTexture(const CameraFramebufferComponent & fb) noexcept {
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb.fbo);
			glClear(GL_COLOR_BUFFER_BIT);

			glBlitFramebuffer(0, 0, (GLint)params.viewport.size.x, (GLint)params.viewport.size.y, 0, 0, (GLint)params.viewport.size.x, (GLint)params.viewport.size.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

			runShaders<LightingShaderComponent>();
			runShaders<PostLightingShaderComponent>();
			runShaders<PostProcessShaderComponent>();
		}

		static void blitTextureToViewport(const CameraFramebufferComponent & fb, const ViewportComponent & viewport) noexcept {
			glBindFramebuffer(GL_READ_FRAMEBUFFER, fb.fbo);
			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

			const auto box = cameraHelper::convertToScreenPercentage({ viewport.boundingBox.position, viewport.boundingBox.size }, window.comp->size, viewport);

			const auto destSizeX = (GLint)(box.size.x * window.comp->size.x);
			const auto destSizeY = (GLint)(box.size.y * window.comp->size.y);

			const auto destX = (GLint)(box.position.x * window.comp->size.x);
			// OpenGL coords have Y=0 at the bottom, I want Y=0 at the top
			const auto destY = (GLint)(window.comp->size.y - destSizeY - box.position.y * window.comp->size.y);

			glBlitFramebuffer(
				// src
				0, 0, fb.resolution.x, fb.resolution.y,
				// dest
				destX, destY, destX + destSizeX, destY + destSizeY,
				GL_COLOR_BUFFER_BIT, GL_LINEAR
			);
		}

		struct GBufferInfo {
			GBufferComponent * gBuffer = nullptr;
			size_t indexForPixel = 0;
		};

		static EntityID getEntityInPixel(EntityID window, const putils::Point2ui & pixel) noexcept {
			static constexpr auto GBUFFER_ENTITY_LOCATION = offsetof(GBufferTextures, entityID) / sizeof(GBufferTextures::entityID);

			const auto info = getGBufferInfo(window, pixel);
			if (info.gBuffer == nullptr)
				return INVALID_ID;

			EntityID ret;
			{ // Release texture asap
				const auto texture = info.gBuffer->getTexture(GBUFFER_ENTITY_LOCATION);
				const auto & size = info.gBuffer->getSize();
				for (size_t i = 0; i < size.x * size.y; ++i)
					if (texture.data[i] != 0.f)
						break;
				ret = (EntityID)texture.data[info.indexForPixel];
			}
			if (ret == 0)
				ret = INVALID_ID;
			return ret;
		}

		static putils::Point3f getPositionInPixel(EntityID window, const putils::Point2ui & pixel) noexcept {
			static constexpr auto GBUFFER_POSITION_LOCATION = offsetof(GBufferTextures, position) / sizeof(GBufferTextures::position);

			const auto info = getGBufferInfo(window, pixel);
			if (info.gBuffer == nullptr)
				return {};

			const auto texture = info.gBuffer->getTexture(GBUFFER_POSITION_LOCATION);
			const auto & size = info.gBuffer->getSize();

			static_assert(sizeof(putils::Point3f) == sizeof(float[3]));
			return texture.data + info.indexForPixel;
		}

		static GBufferInfo getGBufferInfo(EntityID w, const putils::Point2ui & pixel) noexcept {
			static constexpr auto GBUFFER_TEXTURE_COMPONENTS = 4;

			GBufferInfo ret;

			if (w != INVALID_ID && w != window.id)
				return ret;

			const auto viewportInfo = cameraHelper::getViewportForPixel(window.id, pixel);
			if (viewportInfo.camera == INVALID_ID)
				return ret;

			auto camera = entities[viewportInfo.camera];
			const auto gBuffer = camera.tryGet<GBufferComponent>();
			if (!gBuffer)
				return ret;

			const putils::Point2ui gBufferSize = gBuffer->getSize();
			const auto pixelInGBuffer = putils::Point2ui(viewportInfo.viewportPercent * gBufferSize);
			if (pixelInGBuffer.x >= gBufferSize.x || pixelInGBuffer.y > gBufferSize.y || pixelInGBuffer.y == 0)
				return ret;

			ret.gBuffer = gBuffer;
			ret.indexForPixel = (pixelInGBuffer.x + (gBufferSize.y - pixelInGBuffer.y) * gBufferSize.x) * GBUFFER_TEXTURE_COMPONENTS;

			return ret;
		}

		static void initGBuffer(size_t nbAttributes, const functions::GBufferAttributeIterator & iterator) noexcept {
			gBufferTextureCount = nbAttributes;
			gBufferIterator = iterator;

			for (const auto & [e, shader, gbuffer] : entities.with<MyShaderComponent, GBufferShaderComponent>())
				initShader(*shader.shader);
			for (const auto & [e, shader, lighting] : entities.with<MyShaderComponent, LightingShaderComponent>())
				initShader(*shader.shader);
			for (const auto & [e, shader, postLighting] : entities.with<MyShaderComponent, PostLightingShaderComponent>())
				initShader(*shader.shader);
			for (const auto & [e, shader, postProcess] : entities.with<MyShaderComponent, PostProcessShaderComponent>())
				initShader(*shader.shader);

			for (const auto & [e, modelInfo, modelData] : entities.with<MyModelComponent, ModelDataComponent>())
				for (const auto & meshInfo : modelInfo.meshes) {
					glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);

					size_t location = 0;
					for (const auto & attrib : modelData.vertexAttributes)
						registerVertexAttribute(modelData.vertexSize, location++, attrib.offset, attrib.type);
				}
		}

		static void terminate() noexcept {
			ImGui_ImplOpenGL3_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}
	};
}

namespace kengine {
	EntityCreator * OpenGLSystem() noexcept {
		entities += [](Entity & e) noexcept {
			opengl::impl::init(e);
		};

#ifndef KENGINE_NO_DEFAULT_GBUFFER
		kengine::initGBuffer<GBufferTextures>();
#endif

		return [](Entity & e) {
			e += AdjustableComponent{
				"Render/Planes", {
					{ "Near", &opengl::impl::params.nearPlane },
					{ "Far", &opengl::impl::params.farPlane }
				}
			};
		};
	}
}
