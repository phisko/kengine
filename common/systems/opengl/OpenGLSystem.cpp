#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "opengl/Program.hpp"

#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include "EntityManager.hpp"

#include "components/ModelLoaderComponent.hpp"
#include "components/TextureLoaderComponent.hpp"

#include "components/ModelComponent.hpp"
#include "components/OpenGLModelComponent.hpp"
#include "components/ImGuiComponent.hpp"
#include "components/AdjustableComponent.hpp"
#include "components/InputComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/CameraComponent.hpp"
#include "components/ShaderComponent.hpp"
#include "components/SkeletonComponent.hpp"

#include "packets/ImGuiScale.hpp"

#include "common/systems/opengl/ShaderHelper.hpp"

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

#include "Export.hpp"

namespace kengine {
	static bool g_init = false;

	putils::gl::Program::Parameters g_params;
	static auto g_screenChanged = false;
	static float g_dpiScale = 1.f;

	static float * g_entityTexture = nullptr;
	static auto g_entityTextureUpToDate = false;

	static GLFWwindow * g_window = nullptr;

	namespace Input {
		struct KeyInfo {
			int key;
			bool pressed;
		};

		static putils::vector<KeyInfo, 128> keys;

		static putils::Point2f lastPos;
		struct ClickInfo {
			putils::Point2f pos;
			int button;
			bool pressed;
		};
		static putils::vector<ClickInfo, 128> clicks;
		struct MoveInfo {
			putils::Point2f pos;
			putils::Point2f rel;
		};
		static putils::vector<MoveInfo, 128> positions;
		static putils::vector<float, 128> scrolls;

		static void click(GLFWwindow * g_window, int button, int action, int mods) {
			if (clicks.full())
				return;
			if (action == GLFW_PRESS)
				clicks.push_back(ClickInfo{ lastPos, button, true });
			else if (action == GLFW_RELEASE)
				clicks.push_back(ClickInfo{ lastPos, button, false });
		}

		static void move(GLFWwindow * g_window, double xpos, double ypos) {
			if (positions.full())
				return;
			MoveInfo info;
			info.pos = { (float)xpos, (float)ypos };
			info.rel = { xpos - (float)xpos, ypos - (float)ypos };
			positions.push_back(info);
		}

		static void scroll(GLFWwindow * g_window, double xoffset, double yoffset) {
			if (scrolls.full())
				return;
			scrolls.push_back((float)yoffset);
		}

		static void key(GLFWwindow * g_window, int key, int scancode, int action, int mods) {
			if (keys.full())
				return;
			if (action == GLFW_PRESS)
				keys.push_back(KeyInfo{ key, true });
			else if (action == GLFW_RELEASE)
				keys.push_back(KeyInfo{ key, false });
		}
	}

	OpenGLSystem::OpenGLSystem(kengine::EntityManager & em)
		: System(em),
		_em(em)
	{
	}

	void OpenGLSystem::addShaders() noexcept {
		{ // GBuffer
			_em += [=](kengine::Entity & e) { e += kengine::makeGBufferShaderComponent<Shaders::Debug>(_em); };
			_em += [=](kengine::Entity & e) { e += kengine::makeGBufferShaderComponent<Shaders::Text>(_em); };
		}

		{ // Lighting
			_em += [&](kengine::Entity & e) {
				e += kengine::makeLightingShaderComponent<Shaders::ShadowMap>(_em);
				e += kengine::ShadowMapShaderComponent{};
			};

			_em += [&](kengine::Entity & e) {
				e += kengine::makeLightingShaderComponent<Shaders::ShadowCube>(_em);
				e += kengine::ShadowCubeShaderComponent{};
			};

			_em += [=](kengine::Entity & e) { e += kengine::makeLightingShaderComponent<Shaders::SpotLight>(_em); };
			_em += [=](kengine::Entity & e) { e += kengine::makeLightingShaderComponent<Shaders::DirLight>(_em); };
			_em += [=](kengine::Entity & e) { e += kengine::makeLightingShaderComponent<Shaders::PointLight>(_em); };
		}

		{ // Post lighting
			_em += [=](kengine::Entity & e) { e += kengine::makePostLightingShaderComponent<Shaders::GodRaysDirLight>(_em); };
			_em += [=](kengine::Entity & e) { e += kengine::makePostLightingShaderComponent<Shaders::GodRaysPointLight>(_em); };
			_em += [=](kengine::Entity & e) { e += kengine::makePostLightingShaderComponent<Shaders::GodRaysSpotLight>(_em); };
		}

		{ // Post process
			_em += [=](kengine::Entity & e) { e += kengine::makePostProcessShaderComponent<Shaders::LightSphere>(_em); };
			_em += [=](kengine::Entity & e) { e += kengine::makePostProcessShaderComponent<Shaders::Highlight>(_em); };
			_em += [=](kengine::Entity & e) { e += kengine::makePostProcessShaderComponent<Shaders::SkyBox>(_em); };
		}
	}

	void OpenGLSystem::init() noexcept {
		g_init = true;

		g_params.screenSize = { 1280, 720 };
		g_params.nearPlane = 1.f;
		g_params.farPlane = 1000.f;

		g_entityTexture = new float[(int)g_params.screenSize.x * (int)g_params.screenSize.y * 4];

		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

#ifndef KENGINE_NDEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		g_window = glfwCreateWindow((int)g_params.screenSize.x, (int)g_params.screenSize.y, "Kengine", nullptr, nullptr);
		glfwMakeContextCurrent(g_window);
		glfwSetWindowAspectRatio(g_window, (int)g_params.screenSize.x, (int)g_params.screenSize.y);
		glfwSetWindowSizeCallback(g_window, [](auto window, int width, int height) {
			g_params.screenSize.x = (float)width;
			g_params.screenSize.y = (float)height;
			g_screenChanged = true;
			glViewport(0, 0, width, height);
		});

		glfwSetMouseButtonCallback(g_window, Input::click);
		glfwSetCursorPosCallback(g_window, Input::move);
		glfwSetScrollCallback(g_window, Input::scroll);
		glfwSetKeyCallback(g_window, Input::key);

#ifndef KENGINE_NDEBUG
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

		ImGui_ImplGlfw_InitForOpenGL(g_window, true);
		ImGui_ImplOpenGL3_Init();
#endif

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
	}

	void OpenGLSystem::handle(kengine::packets::RegisterEntity p) {
		if (!_gBuffer.isInit())
			return;

		if (p.e.has<kengine::GBufferShaderComponent>())
			initShader(*p.e.get<kengine::GBufferShaderComponent>().shader);

		if (p.e.has<kengine::LightingShaderComponent>())
			initShader(*p.e.get<kengine::LightingShaderComponent>().shader);

		if (p.e.has<kengine::PostLightingShaderComponent>())
			initShader(*p.e.get<kengine::PostLightingShaderComponent>().shader);

		if (p.e.has<kengine::PostProcessShaderComponent>())
			initShader(*p.e.get<kengine::PostProcessShaderComponent>().shader);
	}

	void OpenGLSystem::handle(kengine::packets::DefineGBufferSize p) {
		if (_gBuffer.isInit())
			return;

		_gBuffer.init((size_t)g_params.screenSize.x, (size_t)g_params.screenSize.y, p.nbAttributes);

		for (const auto & [e, shader] : _em.getEntities<kengine::GBufferShaderComponent>())
			initShader(*shader.shader);
		for (const auto & [e, shader] : _em.getEntities<kengine::LightingShaderComponent>())
			initShader(*shader.shader);
		for (const auto & [e, shader] : _em.getEntities<kengine::PostLightingShaderComponent>())
			initShader(*shader.shader);
		for (const auto & [e, shader] : _em.getEntities<kengine::PostProcessShaderComponent>())
			initShader(*shader.shader);

		for (const auto & [e, modelInfo] : _em.getEntities<kengine::OpenGLModelComponent>())
			for (const auto & meshInfo : modelInfo.meshes) {
				glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);
				modelInfo.vertexRegisterFunc();
			}
	}

	void OpenGLSystem::handle(kengine::packets::VertexDataAttributeIterator p) {
		_gBufferIterator = p;
	}

	void OpenGLSystem::handle(kengine::packets::GetGBufferSize p) {
		p.size = _gBuffer.getSize();
	}

	void OpenGLSystem::handle(kengine::packets::GetGBufferTexture p) {
		_gBuffer.getTexture(p.textureIndex, p.buff, p.buffSize);
	}

	void OpenGLSystem::initShader(putils::gl::Program & p) {
		p.init(_gBuffer.getTextureCount(), (size_t)g_params.screenSize.x, (size_t)g_params.screenSize.y, _gBuffer.getFBO());

		assert(_gBufferIterator.func != nullptr);
		int texture = 0;
		_gBufferIterator.func([&](const char * name) {
			p.addGBufferTexture(name, texture++);
		});
	}


	OpenGLSystem::~OpenGLSystem() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(g_window);
		glfwTerminate();
	}

	void OpenGLSystem::onLoad(const char *) noexcept {
		if (!g_init)
			return;

		_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Planes] Near", &g_params.nearPlane); };
		_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Planes] Far", &g_params.farPlane); };

#ifndef KENGINE_OPENGL_NO_DEFAULT_SHADERS
		addShaders();
#endif

#ifndef KENGINE_NDEBUG
		_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[ImGui] Scale", &g_dpiScale); };

		_em += Controllers::ShaderController(_em);
		_em += Controllers::TextureDebugger(_em, _gBuffer, _gBufferIterator);
		_em += Controllers::MouseController(g_window);
#endif

		for (const auto &[e, depthMap] : _em.getEntities<DepthMapComponent>())
			glDeleteFramebuffers(1, &depthMap.fbo);
		for (const auto &[e, depthCube] : _em.getEntities<DepthCubeComponent>())
			glDeleteFramebuffers(1, &depthCube.fbo);

		for (const auto & [e, modelInfo] : _em.getEntities<OpenGLModelComponent>())
			for (const auto & mesh : modelInfo.meshes) {
				glDeleteVertexArrays(1, &mesh.vertexArrayObject);
				glDeleteBuffers(1, &mesh.vertexBuffer);
				glDeleteBuffers(1, &mesh.indexBuffer);
			}
	}

	void OpenGLSystem::createObject(kengine::Entity & e, const kengine::ModelLoaderComponent & modelLoader) {
		const auto modelData = modelLoader.load();

		auto & modelInfo = e.attach<OpenGLModelComponent>();
		modelInfo.meshes.clear();
		modelInfo.vertexRegisterFunc = modelLoader.vertexRegisterFunc;

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

			if (_gBuffer.isInit())
				modelLoader.vertexRegisterFunc();

			meshInfo.nbIndices = meshData.indices.nbElements;
			meshInfo.indexType = meshData.indexType;

			modelInfo.meshes.push_back(meshInfo);
		}

		modelLoader.free();
		e.detach<ModelLoaderComponent>();
	}

	static void loadTexture(Entity & e, TextureLoaderComponent & textureLoader) {
		if (*textureLoader.textureID == -1)
			glGenTextures(1, textureLoader.textureID);

		if (textureLoader.data != nullptr) {
			GLenum format;

			switch (textureLoader.components) {
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

			glBindTexture(GL_TEXTURE_2D, *textureLoader.textureID);
			glTexImage2D(GL_TEXTURE_2D, 0, format, textureLoader.width, textureLoader.height, 0, format, GL_UNSIGNED_BYTE, textureLoader.data);
			glGenerateMipmap(GL_TEXTURE_2D);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			if (textureLoader.free != nullptr)
				textureLoader.free(textureLoader.data);
		}

		e.detach<TextureLoaderComponent>();
	}

	void OpenGLSystem::execute() noexcept {
		static bool first = true;
		if (first) {
			init();
			onLoad("");

			send(packets::ImGuiScale{ g_dpiScale });
			first = false;
		}

		g_entityTextureUpToDate = false;

#ifndef KENGINE_NO_DEFAULT_GBUFFER
		if (!_gBuffer.isInit())
			initGBuffer<GBufferTextures>(_em);
#endif

		glfwPollEvents();

		if (glfwGetWindowAttrib(g_window, GLFW_ICONIFIED)) {
			glfwSwapBuffers(g_window);
			return;
		}

		if (glfwWindowShouldClose(g_window)) {
			_em.running = false;
			return;
		}

		if (g_screenChanged) {
			_gBuffer.resize((size_t)g_params.screenSize.x, (size_t)g_params.screenSize.y);
			delete [] g_entityTexture;
			g_entityTexture = new float[(int)g_params.screenSize.x * (int)g_params.screenSize.y * 4];
			g_screenChanged = false;
		}

		handleInput();

		for (auto &[e, meshLoader] : _em.getEntities<kengine::ModelLoaderComponent>()) {
			createObject(e, meshLoader);
			if (e.componentMask == 0)
				_em.removeEntity(e);
		}

		for (auto &[e, textureLoader] : _em.getEntities<kengine::TextureLoaderComponent>()) {
			loadTexture(e, textureLoader);
			if (e.componentMask == 0)
				_em.removeEntity(e);
		}

		doOpenGL();

#ifndef KENGINE_NDEBUG
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		for (const auto &[e, comp] : _em.getEntities<kengine::ImGuiComponent>())
			comp.display(GImGui);
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
#endif

		glfwSwapBuffers(g_window);
	}

	static void axisToQuaternion(float * quat, const glm::vec3 & axis, float angle) {
		angle = angle * 0.5f;
		const float sinus = std::sin(angle);
		quat[0] = sinus * axis[0];
		quat[1] = sinus * axis[1];
		quat[2] = sinus * axis[2];
		quat[3] = std::cos(angle);
	}

	inline void quaternionToMatrix(float * mat, const float *qs) {
		float q[4];
		float s, xs, ys, zs, wx, wy, wz, xx, xy, xz, yy, yz, zz, den;
		q[0] = -qs[0];
		q[1] = -qs[1];
		q[2] = -qs[2];
		q[3] = qs[3];

		den = (q[0] * q[0] + q[1] * q[1] + q[2] * q[2] + q[3] * q[3]);

		if (den == 0.0f)
			s = 1.0f;
		else
			s = 2.0f / den;

		xs = q[0] * s;   ys = q[1] * s;  zs = q[2] * s;
		wx = q[3] * xs;  wy = q[3] * ys; wz = q[3] * zs;
		xx = q[0] * xs;  xy = q[0] * ys; xz = q[0] * zs;
		yy = q[1] * ys;  yz = q[1] * zs; zz = q[2] * zs;

		mat[0] = 1.0f - (yy + zz);
		mat[3] = xy - wz;
		mat[6] = xz + wy;

		mat[1] = xy + wz;
		mat[4] = 1.0f - (xx + zz);
		mat[7] = yz - wx;

		mat[2] = xz - wy;
		mat[5] = yz + wx;
		mat[8] = 1.0f - (xx + yy);
	}

	static void rotate(glm::vec3 & dst, const glm::vec3 & axis, float angle) {
		float quat[4];
		axisToQuaternion(quat, axis, angle);
		float mat[9];
		quaternionToMatrix(mat, quat);

		float x, y, z;
		x = dst[0] * mat[0] + dst[1] * mat[3] + dst[2] * mat[6];
		y = dst[0] * mat[1] + dst[1] * mat[4] + dst[2] * mat[7];
		z = dst[0] * mat[2] + dst[1] * mat[5] + dst[2] * mat[8];
		dst[0] = x;
		dst[1] = y;
		dst[2] = z;
	}

	void OpenGLSystem::doOpenGL() noexcept {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (const auto &[e, cam, transform] : _em.getEntities<kengine::CameraComponent3f, kengine::TransformComponent3f>()) {
			{
				const auto & pos = transform.boundingBox.position;
				const auto & size = transform.boundingBox.size;
				glViewport(
					(int)pos.x * (int)g_params.screenSize.x, (int)pos.y * (int)g_params.screenSize.y,
					(GLsizei)size.x * (GLsizei)g_params.screenSize.x, (GLsizei)size.y * (GLsizei)g_params.screenSize.y
				);
			}

			g_params.camPos = ShaderHelper::toVec(cam.frustrum.position);
			g_params.camFOV = cam.frustrum.size.y;

			g_params.view = [&] {
				const auto front = glm::normalize(glm::vec3{
					std::cos(cam.yaw) * std::cos(cam.pitch),
					std::sin(cam.pitch),
					std::sin(cam.yaw) * std::cos(cam.pitch)
				});
				const auto right = glm::normalize(glm::cross(front, { 0.f, 1.f, 0.f }));
				auto up = glm::normalize(glm::cross(right, front));
				rotate(up, front, cam.roll);

				return glm::lookAt(g_params.camPos, g_params.camPos + front, up);
			}();

			g_params.proj = glm::perspective(
				g_params.camFOV,
				(float)g_params.screenSize.x / (float)g_params.screenSize.y,
				g_params.nearPlane, g_params.farPlane
			);

			for (const auto &[e, depthMap] : _em.getEntities<DepthMapComponent>()) {
				ShaderHelper::BindFramebuffer b(depthMap.fbo);
				glClear(GL_DEPTH_BUFFER_BIT);
			}

			for (const auto &[e, depthCube] : _em.getEntities<DepthCubeComponent>()) {
				ShaderHelper::BindFramebuffer b(depthCube.fbo);
				glClear(GL_DEPTH_BUFFER_BIT);
			}

			_gBuffer.bindForWriting(); {
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				ShaderHelper::Enable depth(GL_DEPTH_TEST);
				for (const auto &[e, comp] : _em.getEntities<GBufferShaderComponent>())
					if (comp.enabled)
						comp.shader->run(g_params);
			} _gBuffer.bindForReading();

			glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
			glBlitFramebuffer(0, 0, (GLint)g_params.screenSize.x, (GLint)g_params.screenSize.y, 0, 0, (GLint)g_params.screenSize.x, (GLint)g_params.screenSize.y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			for (const auto &[e, comp] : _em.getEntities<LightingShaderComponent>())
				if (comp.enabled)
					comp.shader->run(g_params);

			for (const auto &[e, comp] : _em.getEntities<PostLightingShaderComponent>())
				if (comp.enabled)
					comp.shader->run(g_params);

			for (const auto &[e, comp] : _em.getEntities<PostProcessShaderComponent>())
				if (comp.enabled)
					comp.shader->run(g_params);

#ifndef KENGINE_NDEBUG
			if (Controllers::TEXTURE_TO_DEBUG != -1)
				debugTexture(Controllers::TEXTURE_TO_DEBUG);
#endif
		}
	}

	void OpenGLSystem::debugTexture(GLint texture) {
		static GLuint readFBO = -1;

		if (readFBO == -1)
			glGenFramebuffers(1, &readFBO);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, readFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
		glBlitFramebuffer(0, 0, (GLint)g_params.screenSize.x, (GLint)g_params.screenSize.y, 0, 0, (GLint)g_params.screenSize.x, (GLint)g_params.screenSize.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}

	void OpenGLSystem::handleInput() noexcept {
		for (const auto &[e, comp] : _em.getEntities<kengine::InputComponent>()) {
			if (!ImGui::GetIO().WantCaptureKeyboard)
				for (const auto & e : Input::keys)
					if (comp.onKey != nullptr)
						comp.onKey(e.key, e.pressed);

			if (!ImGui::GetIO().WantCaptureMouse) {
				if (comp.onMouseButton != nullptr)
					for (const auto & click : Input::clicks)
						comp.onMouseButton(click.button, click.pos.x, click.pos.y, click.pressed);

				if (comp.onMouseMove != nullptr)
					for (const auto & pos : Input::positions)
						comp.onMouseMove(pos.pos.x, pos.pos.y, pos.rel.x, pos.rel.y);
				if (comp.onMouseWheel != nullptr)
					for (const auto delta : Input::scrolls)
						comp.onMouseWheel(delta, 0.f, 0.f);
			}
		}
		Input::keys.clear();
		Input::clicks.clear();
		Input::positions.clear();
		Input::scrolls.clear();
	}

	void OpenGLSystem::handle(kengine::packets::GetEntityInPixel p) {
		static constexpr auto GBUFFER_TEXTURE_COMPONENTS = 4;
		static constexpr auto GBUFFER_ENTITY_LOCATION = 3;

		const putils::Point2ui gBufferSize = _gBuffer.getSize();

		if (p.pixel.x >= gBufferSize.x || p.pixel.y > gBufferSize.y || p.pixel.y == 0) {
			p.id = kengine::Entity::INVALID_ID;
			return;
		}

		if (!g_entityTextureUpToDate) {
			_gBuffer.getTexture(GBUFFER_ENTITY_LOCATION, g_entityTexture, gBufferSize.x * gBufferSize.y * GBUFFER_TEXTURE_COMPONENTS);
			g_entityTextureUpToDate = true;
		}

		const auto index = (p.pixel.x + (gBufferSize.y - p.pixel.y) * gBufferSize.x) * GBUFFER_TEXTURE_COMPONENTS;
		p.id = (Entity::ID)g_entityTexture[index];
		if (p.id == 0)
			p.id = kengine::Entity::INVALID_ID;
	}
}
