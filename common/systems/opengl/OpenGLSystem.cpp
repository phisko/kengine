#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include "EntityManager.hpp"
#include "systems/LuaSystem.hpp"

#include "components/ModelLoaderComponent.hpp"
#include "components/ModelComponent.hpp"
#include "components/ModelInfoComponent.hpp"
#include "components/ImGuiComponent.hpp"
#include "components/AdjustableComponent.hpp"
#include "components/InputComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/CameraComponent.hpp"
#include "components/ShaderComponent.hpp"
#include "components/SkeletonComponent.hpp"

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

#include "Export.hpp"

static glm::vec3 toVec(const putils::Point3f & p) { return { p.x, p.y, p.z }; }

EXPORT kengine::ISystem * getSystem(kengine::EntityManager & em) {
	return new kengine::OpenGLSystem(em);
}

namespace kengine {
	static auto SCREEN_WIDTH = 1280;
	static auto SCREEN_HEIGHT = 720;
	static auto SCREEN_CHANGED = false;

	static auto GBUFFER_WIDTH = SCREEN_WIDTH;
	static auto GBUFFER_HEIGHT = SCREEN_HEIGHT;

	static GLFWwindow * window = nullptr;

	namespace Input {
		struct KeyInfo {
			int key;
			bool pressed;
		};
		static putils::vector<KeyInfo, 128> keys;
		static putils::vector<putils::Point2f, 128> positions;
		static putils::vector<float, 128> scrolls;

		static void move(GLFWwindow * window, double xpos, double ypos) {
			if (positions.full())
				return;

			if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
				positions.push_back(putils::Point2f{ (float)xpos, (float)ypos });
		}

		static void scroll(GLFWwindow * window, double xoffset, double yoffset) {
			if (scrolls.full())
				return;

			if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
				scrolls.push_back((float)yoffset);
		}

		static void key(GLFWwindow * window, int key, int scancode, int action, int mods) {
			if (keys.full())
				return;

			if (action == GLFW_PRESS)
				keys.push_back(KeyInfo{ key, true });
			else if (action == GLFW_RELEASE)
				keys.push_back(KeyInfo{ key, false });
		}
	}

	static auto NEAR_PLANE = 1.f;
	static auto FAR_PLANE = 1000.f;

	OpenGLSystem::OpenGLSystem(kengine::EntityManager & em)
		: System(em),
		_em(em)
	{
		initOpenGL();

		onLoad("");

		_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Planes] Near", &NEAR_PLANE); };
		_em += [](kengine::Entity & e) { e += kengine::AdjustableComponent("[Render/Planes] Far", &FAR_PLANE); };
	}

	void OpenGLSystem::addShaders() noexcept {
		{ // Lighting
			Shaders::ShadowMap * shadowMap = nullptr;
			_em += [&](kengine::Entity & e) {
				auto & comp = e.attach<kengine::LightingShaderComponent>();
				comp.shader = std::make_unique<Shaders::ShadowMap>(_em);
				shadowMap = (Shaders::ShadowMap *)comp.shader.get();
			};

			Shaders::ShadowCube * shadowCube = nullptr;
			_em += [&](kengine::Entity & e) {
				auto & comp = e.attach<kengine::LightingShaderComponent>();
				comp.shader = std::make_unique<Shaders::ShadowCube>();
				shadowCube = (Shaders::ShadowCube *)comp.shader.get();
			};

			_em += [=](kengine::Entity & e) { e += kengine::makeLightingShaderComponent<Shaders::SpotLight>(_em, *shadowMap); };
			_em += [=](kengine::Entity & e) { e += kengine::makeLightingShaderComponent<Shaders::DirLight>(_em, *shadowMap); };
			_em += [=](kengine::Entity & e) { e += kengine::makeLightingShaderComponent<Shaders::PointLight>(_em, *shadowCube); };
		}

		{ // Post process
			_em += [=](kengine::Entity & e) { e += kengine::makePostProcessShaderComponent<Shaders::GodRaysDirLight>(_em); };
			_em += [=](kengine::Entity & e) { e += kengine::makePostProcessShaderComponent<Shaders::GodRaysPointLight>(_em); };
			_em += [=](kengine::Entity & e) { e += kengine::makePostProcessShaderComponent<Shaders::GodRaysSpotLight>(_em); };
			_em += [=](kengine::Entity & e) { e += kengine::makePostProcessShaderComponent<Shaders::LightSphere>(_em); };
		}
	}

	void OpenGLSystem::initOpenGL() noexcept {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

#ifndef NDEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Kengine", nullptr, nullptr);
		glfwMakeContextCurrent(window);
		glfwSetWindowAspectRatio(window, SCREEN_WIDTH, SCREEN_HEIGHT);
		glfwSetWindowSizeCallback(window, [](auto window, int width, int height) {
			SCREEN_WIDTH = width;
			SCREEN_HEIGHT = height;
			SCREEN_CHANGED = true;
			glViewport(0, 0, width, height);
		});

		glfwSetCursorPosCallback(window, Input::move);
		glfwSetScrollCallback(window, Input::scroll);
		glfwSetKeyCallback(window, Input::key);

		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init();

		glewExperimental = true;
		assert(glewInit() == GLEW_OK);

#ifndef NDEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar * message, const void * userParam) {
			if (severity != GL_DEBUG_SEVERITY_NOTIFICATION)
				fprintf(stderr, "GL: severity = 0x%x, message = %s\n",
					severity, message);
			assert(type != GL_DEBUG_TYPE_ERROR);
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

		if (p.e.has<kengine::PostProcessShaderComponent>())
			initShader(*p.e.get<kengine::PostProcessShaderComponent>().shader);
	}

	void OpenGLSystem::handle(kengine::packets::GBufferSize p) {
		if (_gBuffer.isInit())
			return;

		_gBuffer.init(GBUFFER_WIDTH, GBUFFER_HEIGHT, p.nbAttributes);

		for (const auto & [e, shader] : _em.getEntities<kengine::GBufferShaderComponent>())
			initShader(*shader.shader);
		for (const auto & [e, shader] : _em.getEntities<kengine::LightingShaderComponent>())
			initShader(*shader.shader);
		for (const auto & [e, shader] : _em.getEntities<kengine::PostProcessShaderComponent>())
			initShader(*shader.shader);

		for (const auto & [e, modelInfo] : _em.getEntities<kengine::ModelInfoComponent>())
			for (const auto & meshInfo : modelInfo.meshes) {
				glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);
				modelInfo.vertexRegisterFunc();
			}
	}

	void OpenGLSystem::handle(kengine::packets::VertexDataAttributeIterator p) {
		_gBufferIterator = p;
	}

	void OpenGLSystem::initShader(putils::gl::Program & p) {
		p.init(_gBuffer.getTextureCount(), SCREEN_WIDTH, SCREEN_HEIGHT, _gBuffer.getFBO());
		p.drawObjects = [this](GLint modelLocation) { drawObjects(modelLocation); };

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

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	static auto TOTAL_TIME = 0;
	static auto KENGINE_TIME = 0;
	static auto IMGUI_TIME = 0;
	static auto GLFW_TIME = 0;
	void OpenGLSystem::onLoad(const char *) noexcept {
#ifndef KENGINE_OPENGL_NO_DEFAULT_SHADERS
		addShaders();
#endif

#ifndef NDEBUG
		_em += [](kengine::Entity & e) {
			e += kengine::ImGuiComponent([] {
				if (ImGui::Begin("Render time")) {
					ImGui::Text("Total: %d", TOTAL_TIME);
					ImGui::Text("Kengine: %d", KENGINE_TIME);
					ImGui::Text("ImGui: %d", IMGUI_TIME);
					ImGui::Text("GLFW: %d", GLFW_TIME);
				}
				ImGui::End();
			});
		};
		_em += ShadersController(_em);
		_em += LightsDebugger(_em);
		_em += TextureDebugger(_em, _gBuffer, _gBufferIterator);
		_em += MouseController(window);
#endif
	}

	void OpenGLSystem::createObject(kengine::Entity & e, const kengine::ModelLoaderComponent & modelLoader) {
		const auto modelData = modelLoader.func();

		auto & modelInfo = e.attach<ModelInfoComponent>();
		modelInfo.translation = toVec(modelData.offsetToCentre);
		modelInfo.scale = toVec(modelData.scale);
		modelInfo.pitch = modelData.pitch;
		modelInfo.yaw = modelData.yaw;
		modelInfo.vertexRegisterFunc = modelLoader.vertexRegisterFunc;

		for (const auto & meshData : modelData.meshes) {
			ModelInfoComponent::Mesh meshInfo;
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
		e.detach<kengine::ModelLoaderComponent>();
	}

	void OpenGLSystem::execute() noexcept {
		if (!_gBuffer.isInit())
			return;

		if (SCREEN_CHANGED) {
			_gBuffer.resize(SCREEN_WIDTH, SCREEN_HEIGHT);
			SCREEN_CHANGED = false;
		}

		using namespace std::chrono;

		if (glfwWindowShouldClose(window)) {
			_em.running = false;
			return;
		}

		for (auto & [e, meshLoader] : _em.getEntities<kengine::ModelLoaderComponent>())
			createObject(e, meshLoader);

#ifndef NDEBUG
		const auto START = system_clock::now();
		auto start = START;
#endif

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		for (const auto &[e, comp] : _em.getEntities<kengine::ImGuiComponent>())
			comp.display(GImGui);
		ImGui::Render();

#ifndef NDEBUG
		auto end = system_clock::now();
		IMGUI_TIME = (int)duration_cast<milliseconds>(end - start).count();
		start = end;
#endif

		doOpenGL();

#ifndef NDEBUG
		end = system_clock::now();
		KENGINE_TIME = (int)duration_cast<milliseconds>(end - start).count();
		start = end;
#endif

		handleInput();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();

#ifndef NDEBUG
		end = system_clock::now();
		GLFW_TIME = (int)duration_cast<milliseconds>(end - start).count();
		TOTAL_TIME = (int)duration_cast<milliseconds>(end - START).count();
#endif
	}

	void OpenGLSystem::doOpenGL() noexcept {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (const auto &[e, cam, transform] : _em.getEntities<kengine::CameraComponent3f, kengine::TransformComponent3f>()) {
			{
				const auto & pos = transform.boundingBox.topLeft;
				const auto & size = transform.boundingBox.size;
				glViewport(
					(int)pos.x * SCREEN_WIDTH, (int)pos.y * SCREEN_HEIGHT,
					(GLsizei)size.x * SCREEN_WIDTH, (GLsizei)size.y * SCREEN_HEIGHT
				);
			}

			const auto camPos = toVec(cam.frustrum.topLeft);

			const auto view = [&] {
				const auto front = glm::normalize(glm::vec3{
					std::cos(cam.yaw) * std::cos(cam.pitch),
					std::sin(cam.pitch),
					std::sin(cam.yaw) * std::cos(cam.pitch)
					});
				const auto right = glm::normalize(glm::cross(front, { 0.f, 1.f, 0.f }));

				const auto up = glm::normalize(glm::cross(right, front));
				return glm::lookAt(camPos, camPos + front, up);
			}();

			const glm::mat4 proj = glm::perspective(
				cam.frustrum.size.y,
				(float)GBUFFER_WIDTH / (float)GBUFFER_HEIGHT,
				NEAR_PLANE, FAR_PLANE
			);

			_gBuffer.bindForWriting();
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			for (const auto &[e, comp] : _em.getEntities<kengine::GBufferShaderComponent>())
				if (comp.enabled)
					comp.shader->run(view, proj, camPos, SCREEN_WIDTH, SCREEN_HEIGHT);
			_gBuffer.bindForReading();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			for (const auto &[e, comp] : _em.getEntities<kengine::LightingShaderComponent>())
				if (comp.enabled)
					comp.shader->run(view, proj, camPos, SCREEN_WIDTH, SCREEN_HEIGHT);

			for (const auto &[e, comp] : _em.getEntities<kengine::PostProcessShaderComponent>())
				if (comp.enabled)
					comp.shader->run(view, proj, camPos, SCREEN_WIDTH, SCREEN_HEIGHT);

			if (TEXTURE_TO_DEBUG != -1)
				debugTexture(TEXTURE_TO_DEBUG);
		}
	}

	void OpenGLSystem::debugTexture(GLint texture) {
		static GLuint readFBO = -1;

		if (readFBO == -1)
			glGenFramebuffers(1, &readFBO);

		glBindFramebuffer(GL_READ_FRAMEBUFFER, readFBO);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
		glBlitFramebuffer(0, 0, GBUFFER_WIDTH, GBUFFER_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}

	static void drawModel(const kengine::ModelInfoComponent & modelInfo) {
		for (const auto & meshInfo : modelInfo.meshes) {
			glBindVertexArray(meshInfo.vertexArrayObject);
			glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);
			glDrawElements(GL_TRIANGLES, meshInfo.nbIndices, meshInfo.indexType, nullptr);
		}
	}

	void OpenGLSystem::drawObjects(GLint modelMatrixLocation) const noexcept {
		for (const auto &[e, model, transform] : _em.getEntities<kengine::ModelComponent, kengine::TransformComponent3f>()) {
			if (e.has<SkeletonComponent>())
				continue; // For now I don't support shadows from animated models

			const auto & modelInfoEntity = _em.getEntity(model.modelInfo);
			if (!modelInfoEntity.has<ModelInfoComponent>())
				continue;
			const auto & modelInfo = modelInfoEntity.get<ModelInfoComponent>();

			glm::mat4 model(1.f);
			const auto & centre = transform.boundingBox.topLeft;
			model = glm::translate(model, toVec(centre));
			model = glm::scale(model, toVec(transform.boundingBox.size));

			model = glm::rotate(model,
				transform.yaw,
				{ 0.f, 1.f, 0.f }
			);

			model = glm::rotate(model,
				transform.pitch,
				{ 1.f, 0.f, 0.f }
			);

			model = glm::rotate(model,
				modelInfo.yaw,
				{ 0.f, 1.f, 0.f }
			);

			model = glm::rotate(model,
				modelInfo.pitch,
				{ 1.f, 0.f, 0.f }
			);

			model = glm::translate(model, -modelInfo.translation); // Re-center
			model = glm::scale(model, modelInfo.scale);

			putils::gl::setUniform(modelMatrixLocation, model);

			drawModel(modelInfo);
		}
	}

	void OpenGLSystem::handleInput() noexcept {
		for (const auto &[e, comp] : _em.getEntities<kengine::InputComponent>()) {
			if (!ImGui::GetIO().WantCaptureKeyboard)
				for (const auto & e : Input::keys)
					if (comp.onKey != nullptr)
						comp.onKey(e.key, e.pressed);

			if (!ImGui::GetIO().WantCaptureMouse)
			{
				for (const auto & pos : Input::positions)
					if (comp.onMouseMove != nullptr)
						comp.onMouseMove(pos.x, pos.y);
				for (const auto delta : Input::scrolls)
					if (comp.onMouseWheel != nullptr)
						comp.onMouseWheel(delta, 0.f, 0.f);
			}
		}
		Input::keys.clear();
		Input::positions.clear();
		Input::scrolls.clear();
	}
}