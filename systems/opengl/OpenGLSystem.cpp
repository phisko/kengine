#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "OpenGLSystem.hpp"
#include "Controllers.hpp"

#include "EntityManager.hpp"

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
#include "data/InputBufferComponent.hpp"
#include "data/AdjustableComponent.hpp"
#include "data/InputComponent.hpp"
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
#include "functions/GetImGuiScale.hpp"
#include "functions/GetEntityInPixel.hpp"
#include "functions/GetPositionInPixel.hpp"
#include "functions/InitGBuffer.hpp"

#include "shaders/shaderHelper.hpp"
#include "helpers/cameraHelper.hpp"
#include "helpers/matrixHelper.hpp"
#include "helpers/assertHelper.hpp"

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

#ifndef KENGINE_MAX_VIEWPORTS
# define KENGINE_MAX_VIEWPORTS 8
#endif

namespace kengine {
	namespace Input {
		static InputBufferComponent * g_buffer;
	}

	static EntityManager * g_em;
	static putils::gl::Program::Parameters g_params;
	static float g_dpiScale = 1.f;

	static size_t g_gBufferTextureCount = 0;
	static functions::GBufferAttributeIterator g_gBufferIterator = nullptr;

	static bool g_init = false;
	struct {
		Entity::ID id = Entity::INVALID_ID;
		GLFWWindowComponent * glfw = nullptr;
		WindowComponent * comp = nullptr;
	} g_window;

#pragma region OpenGLSystem
#pragma region declarations
	static void init() noexcept;
	static void execute(float deltaTime);
	static void onEntityCreated(Entity & e);
	static void onEntityRemoved(Entity & e);
	static void terminate();
	static void onMouseCaptured(Entity::ID window, bool captured);
	static Entity::ID getEntityInPixel(Entity::ID window, const putils::Point2ui & pixel);
	static putils::Point3f getPositionInPixel(Entity::ID window, const putils::Point2ui & pixel);
	static void initGBuffer(size_t nbAttributes, const functions::GBufferAttributeIterator & iterator);
#pragma endregion
	EntityCreator * OpenGLSystem(EntityManager & em) {
		g_em = &em;

		for (const auto & [e, buffer] : em.getEntities<InputBufferComponent>()) {
			Input::g_buffer = &buffer;
			break;
		}

		em += [](Entity & e) {
			e += AdjustableComponent{
				"ImGui", {
					{ "Scale", &g_dpiScale }
				}
			};

			e += functions::InitGBuffer{ initGBuffer };
		};

#if !defined(KENGINE_NDEBUG) && !defined(KENGINE_OPENGL_NO_DEBUG_TOOLS)
		em += opengl::ShaderController(em);
		em += opengl::GBufferDebugger(em, g_gBufferIterator);
#endif

		g_params.nearPlane = 1.f;
		g_params.farPlane = 1000.f;

		init();

#ifndef KENGINE_NO_DEFAULT_GBUFFER
		initGBuffer<GBufferTextures>(*g_em);
#endif

		return [](Entity & e) {
			e += functions::Execute{ execute };
			e += functions::OnEntityCreated{ onEntityCreated };
			e += functions::OnEntityRemoved{ onEntityRemoved };
			e += functions::OnTerminate{ terminate };
			e += functions::OnMouseCaptured{ onMouseCaptured };
			e += functions::GetImGuiScale{ [] { return g_dpiScale; } };
			e += functions::GetEntityInPixel{ getEntityInPixel };
			e += functions::GetPositionInPixel{ getPositionInPixel };

			e += AdjustableComponent{
				"Render/Planes", {
					{ "Near", &g_params.nearPlane },
					{ "Far", &g_params.farPlane }
				}
			};
		};
	}

	static void terminate() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

#pragma region init
#pragma region declarations
	static void initImGui();
	static void addShaders();
#pragma endregion
	static void init() noexcept {
		if (g_window.comp == nullptr) {
			for (const auto & [e, window, glfw] : g_em->getEntities<WindowComponent, GLFWWindowComponent>()) {
				if (!window.assignedSystem.empty())
					continue;
				g_window.id = e.id;
				break;
			}

			if (g_window.id == Entity::INVALID_ID) {
				*g_em += [](Entity & e) {
					g_window.comp = &e.attach<WindowComponent>();
					g_window.comp->name = "Kengine";
					g_window.comp->size = { 1280, 720 };
					g_window.comp->assignedSystem = "OpenGL";
					g_window.id = e.id;
				};
			}
			else {
				auto e = g_em->getEntity(g_window.id);
				g_window.comp = &e.get<WindowComponent>();
				g_window.comp->assignedSystem = "OpenGL";
			}
		}

		if (g_window.glfw == nullptr) {
			g_window.glfw = g_em->getEntity(g_window.id).tryGet<GLFWWindowComponent>();
			if (g_window.glfw == nullptr)
				return;
		}

		g_init = true;

		glewExperimental = true;
		const bool ret = glewInit();
		assert(ret == GLEW_OK);

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

	static void initImGui() {
		g_em->getEntity(g_window.id) += ImGuiContextComponent{
			ImGui::CreateContext()
		};

		auto & io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigViewportsNoTaskBarIcon = true;

		ImGui_ImplGlfw_InitForOpenGL(g_window.glfw->window.get(), true);
		ImGui_ImplOpenGL3_Init();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	static void addShaders() {
		{ // GBuffer
			*g_em += [=](Entity & e) { e += makeGBufferShaderComponent<Shaders::Debug>(*g_em); };
			*g_em += [=](Entity & e) { e += makeGBufferShaderComponent<Shaders::Text>(*g_em); };
		}

		{ // Lighting
			*g_em += [&](Entity & e) {
				e += makeLightingShaderComponent<Shaders::ShadowMap>(*g_em, e);
				e += ShadowMapShaderComponent{};
			};

			*g_em += [&](Entity & e) {
				e += makeLightingShaderComponent<Shaders::ShadowCube>(*g_em);
				e += ShadowCubeShaderComponent{};
			};

			*g_em += [=](Entity & e) { e += makeLightingShaderComponent<Shaders::SpotLight>(*g_em); };
			*g_em += [=](Entity & e) { e += makeLightingShaderComponent<Shaders::DirLight>(*g_em, e); };
			*g_em += [=](Entity & e) { e += makeLightingShaderComponent<Shaders::PointLight>(*g_em); };
		}

		{ // Post lighting
			*g_em += [=](Entity & e) { e += makePostLightingShaderComponent<Shaders::GodRaysDirLight>(*g_em); };
			*g_em += [=](Entity & e) { e += makePostLightingShaderComponent<Shaders::GodRaysPointLight>(*g_em); };
			*g_em += [=](Entity & e) { e += makePostLightingShaderComponent<Shaders::GodRaysSpotLight>(*g_em); };
		}

		{ // Post process
			*g_em += [=](Entity & e) { e += makePostProcessShaderComponent<Shaders::LightSphere>(*g_em, e); };
			*g_em += [=](Entity & e) { e += makePostProcessShaderComponent<Shaders::Highlight>(*g_em); };
			*g_em += [=](Entity & e) { e += makePostProcessShaderComponent<Shaders::SkyBox>(*g_em); };
		}
	}
#pragma endregion init

#pragma region onEntityCreated
#pragma region declarations
	static void initShader(putils::gl::Program & program);
#pragma endregion
	static void onEntityCreated(Entity & e) {
		if (g_gBufferIterator == nullptr)
			return;

		const auto gbuffer = e.tryGet<GBufferShaderComponent>();
		if (gbuffer)
			initShader(*gbuffer->shader);

		const auto lighting = e.tryGet<LightingShaderComponent>();
		if (lighting)
			initShader(*lighting->shader);

		const auto postLighting = e.tryGet<PostLightingShaderComponent>();
		if (postLighting)
			initShader(*postLighting->shader);

		const auto postProcess = e.tryGet<PostProcessShaderComponent>();
		if (postProcess)
			initShader(*postProcess->shader);
	}

	static void initShader(putils::gl::Program & p) {
		p.init(g_gBufferTextureCount);

		assert(g_gBufferIterator != nullptr);
		int texture = 0;
		g_gBufferIterator([&](const char * name) {
			p.addGBufferTexture(name, texture++);
		});
	}
#pragma endregion onEntityCreated

	static void onEntityRemoved(Entity & e) {
		const auto viewport = e.tryGet<ViewportComponent>();
		if (viewport) {
			if (viewport->window == g_window.id) {
				GLuint texture = (GLuint)viewport->renderTexture;
				glDeleteTextures(1, &texture);
			}
		}

		if (e.id != g_window.id)
			return;
		g_window.id = Entity::INVALID_ID;
		g_window.glfw = nullptr;
		g_window.comp = nullptr;
		terminate();
	}

	static void onMouseCaptured(Entity::ID window, bool captured) {
		if (window != Entity::INVALID_ID && window != g_window.id)
			return;

		if (captured) {
			glfwSetInputMode(g_window.glfw->window.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}
		else {
			glfwSetInputMode(g_window.glfw->window.get(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		}
	}

#pragma region execute
#pragma region declarations
	static void createObject(Entity & e, const ModelDataComponent & modelData);
	static void loadTexture(Entity & e, const TextureDataComponent & textureData);
	static void doOpenGL();
#pragma endregion
	static void execute(float deltaTime) {
		if (g_window.id == Entity::INVALID_ID)
			return;

		for (auto &[e, modelData, noOpenGL] : g_em->getEntities<ModelDataComponent, no<SystemSpecificModelComponent<putils::gl::Mesh>>>())
			createObject(e, modelData);

		for (auto & [e, textureData, noTextureModel] : g_em->getEntities<TextureDataComponent, no<SystemSpecificTextureComponent<putils::gl::Texture>>>())
			loadTexture(e, textureData);

		doOpenGL();

		static float lastScale = 1.f;
		ImGui::GetIO().FontGlobalScale = g_dpiScale;
		ImGui::GetStyle().ScaleAllSizes(g_dpiScale / lastScale);
		lastScale = g_dpiScale;

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

		glfwSwapBuffers(g_window.glfw->window.get());

		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}


#pragma region createObject
#pragma region declarations
	static void registerVertexAttribute(size_t vertexSize, size_t location, size_t offset, putils::meta::type_index type);
#pragma endregion
	static void createObject(Entity & e, const ModelDataComponent & modelData) {
		auto & openglModel = e.attach<SystemSpecificModelComponent<putils::gl::Mesh>>();

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
				kengine_assert_failed(*g_em, "Unknown index type");
			else
				openglMesh.indexType = it->second;

			openglModel.meshes.push_back(std::move(openglMesh));
		}
	}

	static void registerVertexAttribute(size_t vertexSize, size_t location, size_t offset, putils::meta::type_index type) {
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
			kengine_assert_failed(*g_em, "Unknown vertex attribute type");
			return;
		}

		glEnableVertexAttribArray((GLuint)location);
		if (it->second.type == GL_FLOAT)
			glVertexAttribPointer((GLuint)location, (GLint)it->second.length, it->second.type, GL_FALSE, (GLsizei)vertexSize, (void *)offset);
		else
			glVertexAttribIPointer((GLuint)location, (GLint)it->second.length, it->second.type, (GLsizei)vertexSize, (void *)offset);
	}
#pragma endregion createObject

	static void loadTexture(Entity & e, const TextureDataComponent & textureData) {
		auto & textureModel = e.attach<SystemSpecificTextureComponent<putils::gl::Texture>>();

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
				assert(false);
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

#pragma region doOpenGL
#pragma region declarations
	static void setupParams(const CameraComponent & cam, const ViewportComponent & viewport);
	static CameraFramebufferComponent * initFramebuffer(Entity & e);
	static void fillGBuffer(EntityManager & em, Entity & e, const ViewportComponent & viewport) noexcept;
	static void renderToTexture(EntityManager & em, const CameraFramebufferComponent & fb) noexcept;
	static void blitTextureToViewport(const CameraFramebufferComponent & fb, const ViewportComponent & viewport);
#pragma endregion
	static void doOpenGL() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		struct ToBlit {
			const CameraFramebufferComponent * fb;
			const ViewportComponent * viewport;
		};
		putils::vector<ToBlit, KENGINE_MAX_VIEWPORTS> toBlit;

		for (auto &[e, cam, viewport] : g_em->getEntities<CameraComponent, ViewportComponent>()) {
			if (viewport.window == Entity::INVALID_ID)
				viewport.window = g_window.id;
			else if (viewport.window != g_window.id)
				continue;

			g_params.viewportID = e.id;
			setupParams(cam, viewport);
			fillGBuffer(*g_em, e, viewport);

			auto fb = e.tryGet<CameraFramebufferComponent>();
			if (!fb || fb->resolution != viewport.resolution) {
				fb = initFramebuffer(e);
				if (!fb)
					continue;
			}

			renderToTexture(*g_em, *fb);
			if (viewport.boundingBox.size.x > 0 && viewport.boundingBox.size.y > 0)
				toBlit.push_back(ToBlit{ fb, &viewport });
		}

		std::sort(toBlit.begin(), toBlit.end(), [](const ToBlit & lhs, const ToBlit & rhs) {
			return lhs.viewport->zOrder < rhs.viewport->zOrder;
		});

		for (const auto & blit : toBlit)
			blitTextureToViewport(*blit.fb, *blit.viewport);
	}

	static void setupParams(const CameraComponent & cam, const ViewportComponent & viewport) {
		g_params.viewport.size = viewport.resolution;
		putils::gl::setViewPort(g_params.viewport);

		g_params.camPos = shaderHelper::toVec(cam.frustum.position);
		g_params.camFOV = cam.frustum.size.y;

		g_params.proj = matrixHelper::getProjMatrix(cam, viewport, g_params.nearPlane, g_params.farPlane);
		g_params.view = matrixHelper::getViewMatrix(cam, viewport);
	}

	static CameraFramebufferComponent * initFramebuffer(Entity & e) {
		auto & viewport = e.get<ViewportComponent>();
		if (viewport.resolution.x == 0 || viewport.resolution.y == 0)
			return nullptr;

		auto & fb = e.attach<CameraFramebufferComponent>();
		fb.resolution = viewport.resolution;

		fb.fbo.generate();
		glBindFramebuffer(GL_FRAMEBUFFER, fb.fbo);

		GLuint texture = (GLuint)viewport.renderTexture;
		if (viewport.renderTexture == (ViewportComponent::RenderTexture)-1)
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

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		viewport.renderTexture = (ViewportComponent::RenderTexture)texture;

		return &fb;
	}

	template<typename Shaders>
	static void runShaders(Shaders && shaders) {
		for (auto & [e, comp] : shaders) {
			if (!cameraHelper::entityAppearsInViewport(e, g_params.viewportID))
				continue;
			if (!comp.enabled)
				continue;

#ifndef KENGINE_NDEBUG
			struct ShaderProfiler {
				ShaderProfiler(Entity & e) {
					_comp = &e.attach<opengl::ShaderProfileComponent>();
					_timer.restart();
				}

				~ShaderProfiler() {
					_comp->executionTime = _timer.getTimeSinceStart().count();
				}

				opengl::ShaderProfileComponent * _comp;
				putils::Timer _timer;
			};
			ShaderProfiler _(e);
#endif
			comp.shader->run(g_params);
		}
	}

	static void fillGBuffer(EntityManager & em, Entity & e, const ViewportComponent & viewport) noexcept {
		auto gbuffer = e.tryGet<GBufferComponent>();
		if (!gbuffer) {
			gbuffer = &e.attach<GBufferComponent>();
			gbuffer->init(viewport.resolution.x, viewport.resolution.y, g_gBufferTextureCount);
		}
		if (gbuffer->getSize() != viewport.resolution)
			gbuffer->resize(viewport.resolution.x, viewport.resolution.y);

		gbuffer->bindForWriting();
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			shaderHelper::Enable depth(GL_DEPTH_TEST);
			runShaders(em.getEntities<GBufferShaderComponent>());
		}
		gbuffer->bindForReading();
	}

	static void renderToTexture(EntityManager & em, const CameraFramebufferComponent & fb) noexcept {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb.fbo);
		glClear(GL_COLOR_BUFFER_BIT);

		glBlitFramebuffer(0, 0, (GLint)g_params.viewport.size.x, (GLint)g_params.viewport.size.y, 0, 0, (GLint)g_params.viewport.size.x, (GLint)g_params.viewport.size.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		runShaders(em.getEntities<LightingShaderComponent>());
		runShaders(em.getEntities<PostLightingShaderComponent>());
		runShaders(em.getEntities<PostProcessShaderComponent>());
	}

	static void blitTextureToViewport(const CameraFramebufferComponent & fb, const ViewportComponent & viewport) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fb.fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		const auto box = cameraHelper::convertToScreenPercentage(viewport.boundingBox, g_window.comp->size, viewport);

		const auto destSizeX = (GLint)(box.size.x * g_window.comp->size.x);
		const auto destSizeY = (GLint)(box.size.y * g_window.comp->size.y);

		const auto destX = (GLint)(box.position.x * g_window.comp->size.x);
		// OpenGL coords have Y=0 at the bottom, I want Y=0 at the top
		const auto destY = (GLint)(g_window.comp->size.y - destSizeY - box.position.y * g_window.comp->size.y);

		glBlitFramebuffer(
			// src
			0, 0, fb.resolution.x, fb.resolution.y,
			// dest
			destX, destY, destX + destSizeX, destY + destSizeY,
			GL_COLOR_BUFFER_BIT, GL_LINEAR
		);
	}
#pragma endregion doOpenGL
#pragma endregion execute


#pragma region getEntityInPixel / getPositionInPixel
#pragma region declarations
	struct GBufferInfo {
		GBufferComponent * gBuffer = nullptr;
		size_t indexForPixel = 0;
	};
	static GBufferInfo getGBufferInfo(Entity::ID window, const putils::Point2ui & pixel);
#pragma endregion
	static Entity::ID getEntityInPixel(Entity::ID window, const putils::Point2ui & pixel) {
		static constexpr auto GBUFFER_ENTITY_LOCATION = offsetof(GBufferTextures, entityID) / sizeof(GBufferTextures::entityID);

		const auto info = getGBufferInfo(window, pixel);
		if (info.gBuffer == nullptr)
			return Entity::INVALID_ID;

		Entity::ID ret;
		{ // Release texture asap
			const auto texture = info.gBuffer->getTexture(GBUFFER_ENTITY_LOCATION);
			const auto & size = info.gBuffer->getSize();
			for (size_t i = 0; i < size.x * size.y; ++i)
				if (texture.data[i] != 0.f)
					break;
			ret = (Entity::ID)texture.data[info.indexForPixel];
		}
		if (ret == 0)
			ret = Entity::INVALID_ID;
		return ret;
	}

	static putils::Point3f getPositionInPixel(Entity::ID window, const putils::Point2ui & pixel) {
		static constexpr auto GBUFFER_POSITION_LOCATION = offsetof(GBufferTextures, position) / sizeof(GBufferTextures::position);

		const auto info = getGBufferInfo(window, pixel);
		if (info.gBuffer == nullptr)
			return {};

		const auto texture = info.gBuffer->getTexture(GBUFFER_POSITION_LOCATION);
		const auto & size = info.gBuffer->getSize();

		static_assert(sizeof(putils::Point3f) == sizeof(float[3]));
		return texture.data + info.indexForPixel;
	}

	static GBufferInfo getGBufferInfo(Entity::ID window, const putils::Point2ui & pixel) {
		static constexpr auto GBUFFER_TEXTURE_COMPONENTS = 4;

		GBufferInfo ret;

		if (window != Entity::INVALID_ID && window != g_window.id)
			return ret;

		const auto viewportInfo = cameraHelper::getViewportForPixel(*g_em, g_window.id, pixel);
		if (viewportInfo.camera == Entity::INVALID_ID)
			return ret;

		auto camera = g_em->getEntity(viewportInfo.camera);
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
#pragma endregion getEntityInPixel / getPositionInPixel

	static void initGBuffer(size_t nbAttributes, const functions::GBufferAttributeIterator & iterator) {
		g_gBufferTextureCount = nbAttributes;
		g_gBufferIterator = iterator;

		for (const auto & [e, shader] : g_em->getEntities<GBufferShaderComponent>())
			initShader(*shader.shader);
		for (const auto & [e, shader] : g_em->getEntities<LightingShaderComponent>())
			initShader(*shader.shader);
		for (const auto & [e, shader] : g_em->getEntities<PostLightingShaderComponent>())
			initShader(*shader.shader);
		for (const auto & [e, shader] : g_em->getEntities<PostProcessShaderComponent>())
			initShader(*shader.shader);

		for (const auto & [e, modelInfo, modelData] : g_em->getEntities<SystemSpecificModelComponent<putils::gl::Mesh>, ModelDataComponent>())
			for (const auto & meshInfo : modelInfo.meshes) {
				glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);

				size_t location = 0;
				for (const auto & attrib : modelData.vertexAttributes)
					registerVertexAttribute(modelData.vertexSize, location++, attrib.offset, attrib.type);
			}
	}
#pragma endregion OpenGLSystem
}
