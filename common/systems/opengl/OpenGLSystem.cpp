#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <PolyVox/CubicSurfaceExtractor.h>
#include <PolyVox/MarchingCubesSurfaceExtractor.h>
#include <PolyVox/Mesh.h>

#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include "EntityManager.hpp"
#include "systems/LuaSystem.hpp"

#include "components/ImGuiComponent.hpp"
#include "components/AdjustableComponent.hpp"
#include "components/InputComponent.hpp"
#include "components/TransformComponent.hpp"
#include "components/CameraComponent.hpp"
#include "components/ShaderComponent.hpp"

#include "OpenGLSystem.hpp"
#include "Controllers.hpp"

#include "Geometry.hpp"
#include "ShadowMap.hpp"
#include "ShadowCube.hpp"
#include "SpotLight.hpp"
#include "DirLight.hpp"
#include "PointLight.hpp"
#include "LightSphere.hpp"
#include "GodRaysFirstPass.hpp"
#include "GodRays.hpp"

#include "Export.hpp"

EXPORT kengine::ISystem * getSystem(kengine::EntityManager & em) {
	return new kengine::OpenGLSystem(em);
}

namespace kengine {
	static auto SCREEN_WIDTH = 1280;
	static auto SCREEN_HEIGHT = 720;

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

		onLoad();

		_em += [](kengine::Entity e) { e += kengine::AdjustableComponent("[Render/Planes] Near", &NEAR_PLANE); };
		_em += [](kengine::Entity e) { e += kengine::AdjustableComponent("[Render/Planes] Far", &FAR_PLANE); };
	}

	void OpenGLSystem::addShaders() noexcept {
		{ // GBuffer
			_em += [](kengine::Entity e) { e += kengine::makeGBufferShaderComponent<Shaders::Geometry>(); };
		}

		{ // Lighting
			Shaders::ShadowMap * shadowMap = nullptr;
			_em += [&](kengine::Entity e) {
				auto & comp = e.attach<kengine::LightingShaderComponent>();
				comp.shader = std::make_unique<Shaders::ShadowMap>(_em);
				shadowMap = (Shaders::ShadowMap *)comp.shader.get();
			};

			Shaders::ShadowCube * shadowCube = nullptr;
			_em += [&](kengine::Entity e) {
				auto & comp = e.attach<kengine::LightingShaderComponent>();
				comp.shader = std::make_unique<Shaders::ShadowCube>();
				shadowCube = (Shaders::ShadowCube *)comp.shader.get();
			};

			Shaders::SSAO * ssao = nullptr;
			_em += [&](kengine::Entity e) {
				auto & comp = e.attach<kengine::LightingShaderComponent>();
				comp.shader = std::make_unique<Shaders::SSAO>(_em);
				ssao = (Shaders::SSAO *)comp.shader.get();
			};

			Shaders::SSAOBlur * ssaoBlur = nullptr;
			_em += [&](kengine::Entity e) {
				auto & comp = e.attach<kengine::LightingShaderComponent>();
				comp.shader = std::make_unique<Shaders::SSAOBlur>();
				ssaoBlur = (Shaders::SSAOBlur *)comp.shader.get();
			};

			_em += [=](kengine::Entity e) { e += kengine::makeLightingShaderComponent<Shaders::SpotLight>(_em, *shadowMap); };
			_em += [=](kengine::Entity e) { e += kengine::makeLightingShaderComponent<Shaders::DirLight>(_em, *shadowMap, *ssao, *ssaoBlur); };
			_em += [=](kengine::Entity e) { e += kengine::makeLightingShaderComponent<Shaders::PointLight>(_em, *shadowCube); };

			Shaders::GodRaysFirstPass * firstPass = nullptr;
			_em += [&](kengine::Entity e) {
				auto & comp = e.attach<kengine::LightingShaderComponent>();
				comp.shader = std::make_unique<Shaders::GodRaysFirstPass>(_em);
				firstPass = (Shaders::GodRaysFirstPass *)comp.shader.get();
			};
			_em += [=](kengine::Entity e) { e += kengine::makeLightingShaderComponent<Shaders::GodRays>(_em, *firstPass); };
		}

		{ // Post process
			_em += [=](kengine::Entity e) { e += kengine::makePostProcessShaderComponent<Shaders::LightSphere>(_em); };
		}
	}

	void OpenGLSystem::initOpenGL() noexcept {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

		glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

		window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Painter", nullptr, nullptr);
		glfwMakeContextCurrent(window);
		glfwSetWindowAspectRatio(window, SCREEN_WIDTH, SCREEN_HEIGHT);
		glfwSetWindowSizeCallback(window, [](auto window, int width, int height) {
			SCREEN_WIDTH = width;
			SCREEN_HEIGHT = height;
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

		_gBuffer.init(GBUFFER_WIDTH, GBUFFER_HEIGHT);
	}

	void OpenGLSystem::handle(kengine::packets::RegisterEntity p) {
		if (p.e.has<kengine::GBufferShaderComponent>())
			initShader(*p.e.get<kengine::GBufferShaderComponent>().shader);

		if (p.e.has<kengine::LightingShaderComponent>())
			initShader(*p.e.get<kengine::LightingShaderComponent>().shader);

		if (p.e.has<kengine::PostProcessShaderComponent>())
			initShader(*p.e.get<kengine::PostProcessShaderComponent>().shader);

		if (p.e.has<MeshInfoComponent>()) {
			const auto & meshInfo = p.e.get<MeshInfoComponent>();

			for (const auto &[e, comp] : _em.getEntities<kengine::GBufferShaderComponent>())
				meshInfo.vertexRegisterFunc(*comp.shader);
			for (const auto &[e, comp] : _em.getEntities<kengine::LightingShaderComponent>())
				meshInfo.vertexRegisterFunc(*comp.shader);
			for (const auto &[e, comp] : _em.getEntities<kengine::PostProcessShaderComponent>())
				meshInfo.vertexRegisterFunc(*comp.shader);
		}
	}

	void OpenGLSystem::initShader(putils::gl::Program & p) {
		p.init(_gBuffer.getTextureCount(), SCREEN_WIDTH, SCREEN_HEIGHT, _gBuffer.getFBO());
		p.drawObjects = [this](GLint modelLocation) { drawObjects(modelLocation); };

		for (const auto &[e, meshInfo] : _em.getEntities<MeshInfoComponent>()) {
			glBindVertexArray(meshInfo.vertexArrayObject);
			meshInfo.vertexRegisterFunc(p);
		}
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
	void OpenGLSystem::onLoad() noexcept {
		addShaders();
		registerMeshInfos();

		_em += [this](kengine::Entity e) {
			e += MeshComponent{ _sphereMeshInfo };
			auto & pos = e.attach<kengine::TransformComponent3f>().boundingBox.topLeft;
			pos = { 0.f, 0.f, 0.f };

			e += kengine::ImGuiComponent([&pos] {
				if (ImGui::Begin("Variables"))
					ImGui::InputFloat3("Sphere pos", pos.raw);
				ImGui::End();
			});
		};

		_em += [this](kengine::Entity e) {
			e += MeshComponent{ _lightMeshInfo };
			auto & pos = e.attach<kengine::TransformComponent3f>().boundingBox.topLeft;
			pos = { 0.f, 11.f, 0.f };
		};

#ifndef NDEBUG
		_em += [](kengine::Entity e) {
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
		_em += LightsDebugger(_em);
		_em += TextureDebugger(_em, _gBuffer);
		_em += MouseController(window);
#endif
	}

	static void createSphereInVolume(PolyVox::RawVolume<VertexData> & volData, float radius) {
		const auto & region = volData.getEnclosingRegion();
		const auto volCenter = region.getCentre();

		for (int z = region.getLowerZ(); z < region.getUpperZ(); ++z) {
			for (int y = region.getLowerY(); y < region.getUpperY(); ++y) {
				for (int x = region.getLowerX(); x < region.getUpperX(); ++x) {
					const decltype(volCenter) currentPos{ x, y, z };
					const auto distToCenter = (currentPos - volCenter).length();

					if (distToCenter <= radius)
						volData.setVoxel(x, y, z, { { 1.f, 1.f, 1.f } });
					else
						volData.setVoxel(x, y, z, {});
				}
			}
		}
	}

	void OpenGLSystem::registerMeshInfos() noexcept {
		static constexpr auto VOLUME_SIZE = 32;
		static constexpr auto SPHERE_RADIUS = 8.f;

		{
			PolyVox::RawVolume<VertexData> volData{ PolyVox::Region{
				{ 0, 0, 0 },
				{ VOLUME_SIZE - 1, VOLUME_SIZE - 1, VOLUME_SIZE - 1 }
			} };
			createSphereInVolume(volData, SPHERE_RADIUS);
			_sphereMeshInfo = createObject(volData);
		}

		{
			PolyVox::RawVolume<VertexData> volData{ PolyVox::Region{
				{ 0, 0, 0 },
				{ VOLUME_SIZE - 1, VOLUME_SIZE - 1, VOLUME_SIZE - 1 }
			} };
			createSphereInVolume(volData, 1.f);
			_lightMeshInfo = createObject(volData);
		}
	}

	kengine::Entity::ID OpenGLSystem::createObject(PolyVox::RawVolume<VertexData> & volData) {
		const auto encodedMesh = PolyVox::extractCubicMesh(&volData, volData.getEnclosingRegion());
		const auto mesh = PolyVox::decodeMesh(encodedMesh);
		using MeshType = decltype(mesh);
		using VertexType = MeshType::VertexType;
		using IndexType = MeshType::IndexType;

		kengine::Entity::ID ret;

		const auto & centre = volData.getEnclosingRegion().getCentre();
		_em += [this, &ret, &mesh, &centre](kengine::Entity e) {
			ret = e.id;

			auto & meshInfo = e.attach<MeshInfoComponent>();

			glGenVertexArrays(1, &meshInfo.vertexArrayObject);
			glBindVertexArray(meshInfo.vertexArrayObject);

			glGenBuffers(1, &meshInfo.vertexBuffer);
			glBindBuffer(GL_ARRAY_BUFFER, meshInfo.vertexBuffer);
			glBufferData(GL_ARRAY_BUFFER, mesh.getNoOfVertices() * sizeof(VertexType), mesh.getRawVertexData(), GL_STATIC_DRAW);

			glGenBuffers(1, &meshInfo.indexBuffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshInfo.indexBuffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.getNoOfIndices() * sizeof(IndexType), mesh.getRawIndexData(), GL_STATIC_DRAW);

			meshInfo.vertexRegisterFunc = [](putils::gl::Program & p) { p.setPolyvoxVertexType<VertexType>(); };

			meshInfo.nbIndices = mesh.getNoOfIndices();
			meshInfo.indexType = sizeof(IndexType) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
			static_assert(sizeof(IndexType) == 2 || sizeof(IndexType) == 4);

			meshInfo.translation = { centre.getX(), centre.getY(), centre.getZ() };
		};

		return ret;
	}

	void OpenGLSystem::execute() noexcept {
		using namespace std::chrono;

		if (glfwWindowShouldClose(window) || glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			_em.running = false;
			return;
		}

		const auto START = system_clock::now();
		auto start = START;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		for (const auto &[e, comp] : _em.getEntities<kengine::ImGuiComponent>())
			comp.display(GImGui);
		ImGui::Render();

		auto end = system_clock::now();
		IMGUI_TIME = duration_cast<milliseconds>(end - start).count();
		start = end;

		doOpenGL();

		end = system_clock::now();
		KENGINE_TIME = duration_cast<milliseconds>(end - start).count();
		start = end;

		handleInput();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		glfwPollEvents();

		end = system_clock::now();
		GLFW_TIME = duration_cast<milliseconds>(end - start).count();
		TOTAL_TIME = duration_cast<milliseconds>(end - START).count();
	}

	static glm::vec3 toVec(const putils::Point3f & p) { return { p.x, p.y, p.z }; }

	void OpenGLSystem::doOpenGL() noexcept {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (const auto &[e, cam, transform] : _em.getEntities<kengine::CameraComponent3f, kengine::TransformComponent3f>()) {
			{
				const auto & pos = transform.boundingBox.topLeft;
				const auto & size = transform.boundingBox.size;
				glViewport(
					pos.x * SCREEN_WIDTH, pos.y * SCREEN_HEIGHT,
					(pos.x + size.x) * SCREEN_WIDTH, (pos.y + size.y) * SCREEN_HEIGHT
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
			for (const auto &[e, comp] : _em.getEntities<kengine::GBufferShaderComponent>())
				comp.shader->run(view, proj, camPos, SCREEN_WIDTH, SCREEN_HEIGHT);
			_gBuffer.bindForReading();

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			for (const auto &[e, comp] : _em.getEntities<kengine::LightingShaderComponent>())
				comp.shader->run(view, proj, camPos, SCREEN_WIDTH, SCREEN_HEIGHT);

			for (const auto &[e, comp] : _em.getEntities<kengine::PostProcessShaderComponent>())
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
		glBlitFramebuffer(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	}

	void OpenGLSystem::drawObjects(GLint modelMatrixLocation) const noexcept {
		for (const auto &[e, mesh, transform] : _em.getEntities<MeshComponent, kengine::TransformComponent3f>()) {
			const auto & meshInfo = _em.getEntity(mesh.meshInfo).get<MeshInfoComponent>();

			glm::mat4 model(1.f);
			const auto & centre = transform.boundingBox.topLeft;
			model = glm::translate(model, { centre.x, centre.y, centre.z });
			model = glm::scale(model, { transform.boundingBox.size.x, transform.boundingBox.size.y, transform.boundingBox.size.z });
			model = glm::rotate(model,
				transform.yaw,
				{ 0.f, 1.f, 0.f }
			);
			model = glm::translate(model, -meshInfo.translation); // Re-center

			putils::gl::setUniform(modelMatrixLocation, model);

			glBindVertexArray(meshInfo.vertexArrayObject);
			glDrawElements(GL_TRIANGLES, meshInfo.nbIndices, meshInfo.indexType, nullptr);
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