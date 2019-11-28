#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "opengl/Program.hpp"

#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include "EntityManager.hpp"

#include "systems/InputSystem.hpp"

#include "components/ModelDataComponent.hpp"
#include "components/TextureDataComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/OpenGLModelComponent.hpp"
#include "components/ImGuiComponent.hpp"
#include "components/AdjustableComponent.hpp"
#include "components/InputComponent.hpp"
#include "components/CameraComponent.hpp"
#include "components/ViewportComponent.hpp"
#include "components/WindowComponent.hpp"
#include "components/ShaderComponent.hpp"
#include "components/GBufferComponent.hpp"

#include "packets/ImGuiScale.hpp"

#include "common/systems/opengl/ShaderHelper.hpp"

#include "helpers/CameraHelper.hpp"
#include "rotate.hpp"

#include "OpenGLSystem.hpp"
#include "Controllers.hpp"

#include "ShadowMap.hpp"
#include "ShadowCube.hpp"
#include "SpotLight.hpp"
#include "DirLight.hpp"
#include "PointLight.hpp"
#include "LightSphere.hpp"
#include "GodRaysDirLight.hpp"
#include "GodRaysPointLight.hpp"
#include "GodRaysSpotLight.hpp"
#include "Highlight.hpp"
#include "Debug.hpp"
#include "SkyBox.hpp"
#include "Text.hpp"

namespace kengine {
	static bool g_init = false;

	static size_t g_gBufferTextureCount = 0;

	static struct {
		GLFWwindow * window = nullptr;

		Entity::ID id = Entity::INVALID_ID;
		WindowComponent::string name;
		WindowComponent * comp = nullptr;
		putils::Point2i size;
		bool fullScreen;
	} g_window;

	static putils::gl::Program::Parameters g_params;
	static float g_dpiScale = 1.f;

	namespace Input {
		static InputSystem * g_inputSystem;

		static void onKey(GLFWwindow *, int key, int scancode, int action, int mods) {
			if (GImGui != nullptr && ImGui::GetIO().WantCaptureKeyboard)
				return;

			if (action == GLFW_PRESS)
				g_inputSystem->addEvent(InputSystem::KeyEvent{ g_window.id, key, true });
			else if (action == GLFW_RELEASE)
				g_inputSystem->addEvent(InputSystem::KeyEvent{ g_window.id, key, false });
		}

		static putils::Point2f lastPos{ FLT_MAX, FLT_MAX };

		static void onClick(GLFWwindow *, int button, int action, int mods) {
			if (GImGui != nullptr && ImGui::GetIO().WantCaptureMouse)
				return;

			if (action == GLFW_PRESS)
				g_inputSystem->addEvent(InputSystem::ClickEvent{ g_window.id, lastPos, button, true });
			else if (action == GLFW_RELEASE)
				g_inputSystem->addEvent(InputSystem::ClickEvent{ g_window.id, lastPos, button, false });
		}

		static void onMouseMove(GLFWwindow *, double xpos, double ypos) {
			if (lastPos.x == FLT_MAX) {
				lastPos.x = (float)xpos;
				lastPos.y = (float)ypos;
			}

			InputSystem::MouseMoveEvent info;
			info.window = g_window.id;
			info.pos = { (float)xpos, (float)ypos };
			info.rel = { (float)xpos - lastPos.x, (float)ypos - lastPos.y };
			lastPos = info.pos;

			if (GImGui != nullptr && ImGui::GetIO().WantCaptureMouse)
				return;
			g_inputSystem->addEvent(info);
		}

		static void onScroll(GLFWwindow *, double xoffset, double yoffset) {
			if (GImGui != nullptr && ImGui::GetIO().WantCaptureMouse)
				return;
			g_inputSystem->addEvent(InputSystem::MouseScrollEvent{ g_window.id, (float)xoffset, (float)yoffset, lastPos });
		}
	}

	OpenGLSystem::OpenGLSystem(EntityManager & em)
		: System(em),
		_em(em)
	{
		Input::g_inputSystem = &em.getSystem<InputSystem>();

		g_params.nearPlane = 1.f;
		g_params.farPlane = 1000.f;

		_em += [](Entity & e) { e += AdjustableComponent("[Render/Planes] Near", &g_params.nearPlane); };
		_em += [](Entity & e) { e += AdjustableComponent("[Render/Planes] Far", &g_params.farPlane); };
		_em += [](Entity & e) { e += AdjustableComponent("[ImGui] Scale", &g_dpiScale); };

#ifndef KENGINE_NDEBUG
		_em += Controllers::ShaderController(_em);
		_em += Controllers::GBufferDebugger(_em, _gBufferIterator);
#endif
	}

	OpenGLSystem::~OpenGLSystem() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(g_window.window);
		glfwTerminate();
	}

	static void initWindow() noexcept {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#ifndef KENGINE_NDEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
		// TODO: depend on g_windowComponent->fullscreen
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		g_window.window = glfwCreateWindow((int)g_window.comp->size.x, (int)g_window.comp->size.y, g_window.comp->name, nullptr, nullptr);
		// Desired size may not have been available, update to actual size
		glfwGetWindowSize(g_window.window, &g_window.size.x, &g_window.size.y);
		g_window.comp->size = g_window.size;
		glfwSetWindowAspectRatio(g_window.window, g_window.size.x, g_window.size.y);

		glfwMakeContextCurrent(g_window.window);
		glfwSetWindowSizeCallback(g_window.window, [](auto window, int width, int height) {
			g_window.size = { width, height };
			g_window.comp->size = g_window.size;
		});

		glfwSetMouseButtonCallback(g_window.window, Input::onClick);
		glfwSetCursorPosCallback(g_window.window, Input::onMouseMove);
		glfwSetScrollCallback(g_window.window, Input::onScroll);
		glfwSetKeyCallback(g_window.window, Input::onKey);
	}

	static void initImGui() noexcept {
		ImGui::CreateContext();
		auto & io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigViewportsNoTaskBarIcon = true;

		{ // Stolen from ImGui_ImplOpenGL3_CreateFontsTexture
			ImFontConfig config;
			config.SizePixels = 13.f * g_dpiScale;
			io.Fonts->AddFontDefault(&config);
			unsigned char * pixels;
			int width, height;
			io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

			GLint last_texture;
			glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
			glBindTexture(GL_TEXTURE_2D, (GLuint)io.Fonts->TexID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
			ImGui::GetStyle().ScaleAllSizes(g_dpiScale);
			glBindTexture(GL_TEXTURE_2D, last_texture);
		}

		ImGui_ImplGlfw_InitForOpenGL(g_window.window, true);
		ImGui_ImplOpenGL3_Init();
	}

	void OpenGLSystem::init() noexcept {
		g_init = true;

		for (const auto & [e, window] : _em.getEntities<WindowComponent>()) {
			if (!window.assignedSystem.empty())
				continue;
			g_window.id = e.id;
			break;
		}

		if (g_window.id == Entity::INVALID_ID) {
			_em += [](Entity & e) {
				g_window.comp = &e.attach<WindowComponent>();
				g_window.comp->name = "Kengine";
				g_window.comp->size = { 1280, 720 };
				g_window.id = e.id;
			};
		}
		else
			g_window.comp = &_em.getEntity(g_window.id).get<WindowComponent>();

		g_window.comp->assignedSystem = "OpenGL";

		g_window.name = g_window.comp->name;

		initWindow();
		initImGui();

		glewExperimental = true;
		const bool ret = glewInit();
		assert(ret == GLEW_OK);

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

	void OpenGLSystem::addShaders() noexcept {
		{ // GBuffer
			_em += [=](Entity & e) { e += makeGBufferShaderComponent<Shaders::Debug>(_em); };
			_em += [=](Entity & e) { e += makeGBufferShaderComponent<Shaders::Text>(_em); };
		}

		{ // Lighting
			_em += [&](Entity & e) {
				e += makeLightingShaderComponent<Shaders::ShadowMap>(_em);
				e += ShadowMapShaderComponent{};
			};

			_em += [&](Entity & e) {
				e += makeLightingShaderComponent<Shaders::ShadowCube>(_em);
				e += ShadowCubeShaderComponent{};
			};

			_em += [=](Entity & e) { e += makeLightingShaderComponent<Shaders::SpotLight>(_em); };
			_em += [=](Entity & e) { e += makeLightingShaderComponent<Shaders::DirLight>(_em); };
			_em += [=](Entity & e) { e += makeLightingShaderComponent<Shaders::PointLight>(_em); };
		}

		{ // Post lighting
			_em += [=](Entity & e) { e += makePostLightingShaderComponent<Shaders::GodRaysDirLight>(_em); };
			_em += [=](Entity & e) { e += makePostLightingShaderComponent<Shaders::GodRaysPointLight>(_em); };
			_em += [=](Entity & e) { e += makePostLightingShaderComponent<Shaders::GodRaysSpotLight>(_em); };
		}

		{ // Post process
			_em += [=](Entity & e) { e += makePostProcessShaderComponent<Shaders::LightSphere>(_em); };
			_em += [=](Entity & e) { e += makePostProcessShaderComponent<Shaders::Highlight>(_em); };
			_em += [=](Entity & e) { e += makePostProcessShaderComponent<Shaders::SkyBox>(_em); };
		}
	}

	void OpenGLSystem::handle(packets::RegisterEntity p) {
		if (_gBufferIterator.func == nullptr)
			return;

		if (p.e.has<GBufferShaderComponent>())
			initShader(*p.e.get<GBufferShaderComponent>().shader);

		if (p.e.has<LightingShaderComponent>())
			initShader(*p.e.get<LightingShaderComponent>().shader);

		if (p.e.has<PostLightingShaderComponent>())
			initShader(*p.e.get<PostLightingShaderComponent>().shader);

		if (p.e.has<PostProcessShaderComponent>())
			initShader(*p.e.get<PostProcessShaderComponent>().shader);
	}

	void OpenGLSystem::handle(packets::DefineGBufferSize p) {
		g_gBufferTextureCount = p.nbAttributes;

		for (const auto & [e, shader] : _em.getEntities<GBufferShaderComponent>())
			initShader(*shader.shader);
		for (const auto & [e, shader] : _em.getEntities<LightingShaderComponent>())
			initShader(*shader.shader);
		for (const auto & [e, shader] : _em.getEntities<PostLightingShaderComponent>())
			initShader(*shader.shader);
		for (const auto & [e, shader] : _em.getEntities<PostProcessShaderComponent>())
			initShader(*shader.shader);

		for (const auto & [e, modelInfo] : _em.getEntities<OpenGLModelComponent>())
			for (const auto & meshInfo : modelInfo.meshes) {
				glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);
				modelInfo.vertexRegisterFunc();
			}
	}

	void OpenGLSystem::handle(packets::RemoveEntity p) {
		if (!p.e.has<WindowComponent>() || p.e.id != g_window.id)
			return;
		g_window.id = Entity::INVALID_ID;
		g_window.comp = nullptr;
		glfwDestroyWindow(g_window.window);
	}

	void OpenGLSystem::handle(packets::GBufferTexturesIterator p) {
		_gBufferIterator = p;
	}

	void OpenGLSystem::handle(packets::CaptureMouse p) {
		if (p.window != Entity::INVALID_ID && p.window != g_window.id)
			return;

		if (p.captured) {
			glfwSetInputMode(g_window.window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
		}
		else {
			glfwSetInputMode(g_window.window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		}
	}

	void OpenGLSystem::initShader(putils::gl::Program & p) {
		p.init(g_gBufferTextureCount);

		assert(_gBufferIterator.func != nullptr);
		int texture = 0;
		_gBufferIterator.func([&](const char * name) {
			p.addGBufferTexture(name, texture++);
		});
	}

	static void createObject(Entity & e, const ModelDataComponent & modelData) {
		auto & modelInfo = e.attach<OpenGLModelComponent>();
		modelInfo.meshes.clear();
		modelInfo.vertexRegisterFunc = modelData.vertexRegisterFunc;

		for (const auto & meshData : modelData.meshes) {
			OpenGLModelComponent::Mesh meshInfo;
			glGenVertexArrays(1, &meshInfo.vertexArrayObject);
			glBindVertexArray(meshInfo.vertexArrayObject);

			glGenBuffers(1, &meshInfo.vertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, meshData.vertices.nbElements * meshData.vertices.elementSize, meshData.vertices.data, GL_STATIC_DRAW);

			glGenBuffers(1, &meshInfo.indexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshInfo.indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshData.indices.nbElements * meshData.indices.elementSize, meshData.indices.data, GL_STATIC_DRAW);

			modelData.vertexRegisterFunc();

			meshInfo.nbIndices = meshData.indices.nbElements;
			meshInfo.indexType = meshData.indexType;

			modelInfo.meshes.push_back(meshInfo);
		}

		modelData.free();
		e.detach<ModelDataComponent>();
	}

	static void loadTexture(Entity & e, TextureDataComponent & textureData) {
		if (*textureData.textureID == -1)
			glGenTextures(1, textureData.textureID);

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

			glBindTexture(GL_TEXTURE_2D, *textureData.textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, textureData.width, textureData.height, 0, format, GL_UNSIGNED_BYTE, textureData.data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			if (textureData.free != nullptr)
				textureData.free(textureData.data);
		}

		e.detach<TextureDataComponent>();
	}

	void OpenGLSystem::execute() noexcept {
		static bool first = true;
		if (first) {
			init();

			send(packets::ImGuiScale{ g_dpiScale });
#ifndef KENGINE_NO_DEFAULT_GBUFFER
			initGBuffer<GBufferTextures>(_em);
#endif
			first = false;
		}

		if (g_window.id == Entity::INVALID_ID)
			return;

		glfwPollEvents();
		updateWindowProperties();

		for (auto &[e, modelData] : _em.getEntities<ModelDataComponent>()) {
			createObject(e, modelData);
			if (e.componentMask == 0)
				_em.removeEntity(e);
		}

		for (auto &[e, textureLoader] : _em.getEntities<TextureDataComponent>()) {
			loadTexture(e, textureLoader);
			if (e.componentMask == 0)
				_em.removeEntity(e);
		}

		doOpenGL();
		doImGui();
		glfwSwapBuffers(g_window.window);
	}

	void OpenGLSystem::updateWindowProperties() noexcept {
		if (glfwGetWindowAttrib(g_window.window, GLFW_ICONIFIED)) {
			glfwSwapBuffers(g_window.window);
			return;
		}

		if (glfwWindowShouldClose(g_window.window)) {
			if (g_window.comp->shutdownOnClose)
				_em.running = false;
			_em.getEntity(g_window.id).detach<WindowComponent>();
			g_window.id = Entity::INVALID_ID;
			g_window.comp = nullptr;
			return;
		}
		
		if (g_window.comp->name != g_window.name) {
			g_window.name = g_window.comp->name;
			glfwSetWindowTitle(g_window.window, g_window.comp->name);
		}

		if (g_window.comp->size != g_window.size) {
			g_window.size = g_window.comp->size;
			glfwSetWindowSize(g_window.window, g_window.size.x, g_window.size.y);
			glfwGetWindowSize(g_window.window, &g_window.size.x, &g_window.size.y);
			g_window.comp->size = g_window.size;
			glfwSetWindowAspectRatio(g_window.window, g_window.size.x, g_window.size.y);
		}

		if (g_window.comp->fullscreen != g_window.fullScreen) {
			g_window.fullScreen = g_window.comp->fullscreen;

			const auto monitor = glfwGetPrimaryMonitor();
			const auto mode = glfwGetVideoMode(monitor);

			if (g_window.comp->fullscreen)
				glfwSetWindowMonitor(g_window.window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
			else
				glfwSetWindowMonitor(g_window.window, nullptr, 0, 0, g_window.size.x, g_window.size.y, mode->refreshRate);

			glfwGetWindowSize(g_window.window, &g_window.size.x, &g_window.size.y);
			g_window.comp->size = g_window.size;
			glfwSetWindowAspectRatio(g_window.window, g_window.size.x, g_window.size.y);
		}
	}

	struct CameraFramebufferComponent {
		GLuint fbo = -1;
		GLuint depthTexture = -1;
		putils::Point2i resolution;
	};

	static void initFramebuffer(Entity & e) {
		auto & viewport = e.get<ViewportComponent>();
		if (viewport.resolution.x == 0 || viewport.resolution.y == 0)
			return;

		auto & fb = e.attach<CameraFramebufferComponent>();
		fb.resolution = viewport.resolution;

		if (fb.fbo == -1)
			glGenFramebuffers(1, &fb.fbo);
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

		if (fb.depthTexture == -1)
			glGenTextures(1, &fb.depthTexture);
		glBindTexture(GL_TEXTURE_2D, fb.depthTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, viewport.resolution.x, viewport.resolution.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fb.depthTexture, 0);

		assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

		viewport.renderTexture = (ViewportComponent::RenderTexture)texture;
	}

	void setupParams(const CameraComponent3f & cam, const ViewportComponent & viewport) {
		g_params.viewPort.size = viewport.resolution;
		putils::gl::setViewPort(g_params.viewPort);

		g_params.camPos = ShaderHelper::toVec(cam.frustum.position);
		g_params.camFOV = cam.frustum.size.y;

		g_params.view = [&] {
			const auto front = glm::normalize(glm::vec3{
				std::sin(cam.yaw) * std::cos(cam.pitch),
				std::sin(cam.pitch),
				std::cos(cam.yaw) * std::cos(cam.pitch)
				});
			const auto right = glm::normalize(glm::cross(front, { 0.f, 1.f, 0.f }));
			auto up = glm::normalize(glm::cross(right, front));
			detail::rotate(up, front, cam.roll);

			return glm::lookAt(g_params.camPos, g_params.camPos + front, up);
		}();

		g_params.proj = glm::perspective(
			g_params.camFOV,
			(float)g_params.viewPort.size.x / (float)g_params.viewPort.size.y,
			g_params.nearPlane, g_params.farPlane
		);
	}

	template<typename Shaders>
	static void runShaders(Shaders && shaders) {
		for (auto & [e, comp] : shaders)
			if (comp.enabled) {
#ifndef KENGINE_NDEBUG
				struct ShaderProfiler {
					ShaderProfiler(Entity & e) {
						_comp = &e.attach<Controllers::ShaderProfileComponent>();
						_timer.restart();
					}

					~ShaderProfiler() {
						_comp->executionTime = _timer.getTimeSinceStart().count();
					}

					Controllers::ShaderProfileComponent * _comp;
					putils::Timer _timer;
				};
				ShaderProfiler _(e);
#endif
				comp.shader->run(g_params);
			}
	}

	static void fillGBuffer(EntityManager & em, Entity & e, const ViewportComponent & viewport) noexcept {
		if (!e.has<GBufferComponent>()) {
			auto & gbuffer = e.attach<GBufferComponent>();
			gbuffer.init(viewport.resolution.x, viewport.resolution.y, g_gBufferTextureCount);
		}
		auto & gbuffer = e.get<GBufferComponent>();
		if (gbuffer.getSize() != viewport.resolution)
			gbuffer.resize(viewport.resolution.x, viewport.resolution.y);

		gbuffer.bindForWriting();
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			ShaderHelper::Enable depth(GL_DEPTH_TEST);
			runShaders(em.getEntities<GBufferShaderComponent>());
		}
		gbuffer.bindForReading();
	}

	static void renderToTexture(EntityManager & em, const CameraFramebufferComponent & fb) noexcept {
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb.fbo);
		glClear(GL_COLOR_BUFFER_BIT);

		glBlitFramebuffer(0, 0, (GLint)g_params.viewPort.size.x, (GLint)g_params.viewPort.size.y, 0, 0, (GLint)g_params.viewPort.size.x, (GLint)g_params.viewPort.size.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

		runShaders(em.getEntities<LightingShaderComponent>());
		runShaders(em.getEntities<PostLightingShaderComponent>());
		runShaders(em.getEntities<PostProcessShaderComponent>());
	}

	static void blitTextureToViewport(const CameraFramebufferComponent & fb, const ViewportComponent & viewport) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, fb.fbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

		const auto destSizeX = (GLint)(viewport.boundingBox.size.x * g_window.size.x);
		const auto destSizeY = (GLint)(viewport.boundingBox.size.y * g_window.size.y);

		const auto destX = (GLint)(viewport.boundingBox.position.x * g_window.size.x);
		// OpenGL coords have Y=0 at the bottom, I want Y=0 at the top
		const auto destY = (GLint)(g_window.size.y - destSizeY - viewport.boundingBox.position.y * g_window.size.y);

		glBlitFramebuffer(
			// src
			0, 0, fb.resolution.x, fb.resolution.y,
			// dest
			destX, destY, destX + destSizeX, destY + destSizeY,
			GL_COLOR_BUFFER_BIT, GL_LINEAR
		);
	}

	void OpenGLSystem::doOpenGL() noexcept {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		struct ToBlit {
			const CameraFramebufferComponent * fb;
			const ViewportComponent * viewport;
		};
		putils::vector<ToBlit, KENGINE_MAX_VIEWPORTS> toBlit;

		for (auto &[e, cam, viewport] : _em.getEntities<CameraComponent3f, ViewportComponent>()) {
			if (viewport.window == Entity::INVALID_ID)
				viewport.window = g_window.id;
			else if (viewport.window != g_window.id)
				return;

			setupParams(cam, viewport);
			fillGBuffer(_em, e, viewport);

			if (!e.has<CameraFramebufferComponent>() || e.get<CameraFramebufferComponent>().resolution != viewport.resolution)
				initFramebuffer(e);
			auto & fb = e.get<CameraFramebufferComponent>();

			renderToTexture(_em, fb);
			if (viewport.boundingBox.size.x > 0 && viewport.boundingBox.size.y > 0)
				toBlit.push_back(ToBlit{ &fb, &viewport });
		}

		std::sort(toBlit.begin(), toBlit.end(), [](const ToBlit & lhs, const ToBlit & rhs) {
			return lhs.viewport->zOrder < rhs.viewport->zOrder;
		});

		for (const auto & blit : toBlit)
			blitTextureToViewport(*blit.fb, *blit.viewport);
	}

	void OpenGLSystem::doImGui() noexcept {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		for (const auto &[e, comp] : _em.getEntities<ImGuiComponent>())
			comp.display(GImGui);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}

	void OpenGLSystem::handle(packets::GetEntityInPixel p) {
		static constexpr auto GBUFFER_TEXTURE_COMPONENTS = 4;
		static constexpr auto GBUFFER_ENTITY_LOCATION = 3;

		if (p.window != Entity::INVALID_ID && p.window != g_window.id)
			return;

		const auto viewportInfo = CameraHelper::getViewportForPixel(_em, g_window.id, p.pixel);
		if (viewportInfo.camera == Entity::INVALID_ID) {
			p.id = Entity::INVALID_ID;
			return;
		}

		auto & camera = _em.getEntity(viewportInfo.camera);
		if (!camera.has<GBufferComponent>()) {
			p.id = Entity::INVALID_ID;
			return;
		}

		auto & gbuffer = camera.get<GBufferComponent>();

		const putils::Point2ui gBufferSize = gbuffer.getSize();
		const auto pixelInGBuffer = putils::Point2i(viewportInfo.pixel * gBufferSize);
		if (pixelInGBuffer.x >= gBufferSize.x || pixelInGBuffer.y > gBufferSize.y || pixelInGBuffer.y == 0) {
			p.id = Entity::INVALID_ID;
			return;
		}

		const auto index = (pixelInGBuffer.x + (gBufferSize.y - pixelInGBuffer.y) * gBufferSize.x) * GBUFFER_TEXTURE_COMPONENTS;
		{ // Release texture asap
			const auto texture = gbuffer.getTexture(GBUFFER_ENTITY_LOCATION);
			p.id = (Entity::ID)texture.data[index];
		}
		if (p.id == 0)
			p.id = Entity::INVALID_ID;
	}
}
