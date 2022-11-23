#include "GLFWSystem.hpp"

// entt
#include <entt/entity/registry.hpp>

// glfw
#include <GLFW/glfw3.h>

// imgui
#include <imgui.h>

// kengine data
#include "data/GLFWWindowComponent.hpp"
#include "data/InputBufferComponent.hpp"
#include "data/WindowComponent.hpp"

// kengine functions
#include "functions/OnTerminate.hpp"
#include "functions/OnMouseCaptured.hpp"
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/assertHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	struct GLFWSystem {
		static void init(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "GLFWSystem");

			_r = &r;

			const auto e = r.create();
			r.emplace<functions::Execute>(e, execute);
			r.emplace<functions::OnTerminate>(e, terminate);
			r.emplace<functions::OnMouseCaptured>(e, onMouseCaptured);

			for (const auto & [bufferEntity, buffer] : r.view<InputBufferComponent>().each()) {
				Input::_buffer = &buffer;
				break;
			}

			glfwSetErrorCallback([](int error, const char * desc) {
				kengine_logf(*_r, Error, "GLFW", "Error code: %d. Description: '%s'", error, desc);
			});

			const auto ret = glfwInit();
			kengine_assert(r, ret == GLFW_TRUE);
			execute(0.f); // init already existing windows
		}

		static void terminate() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(*_r, Log, "Terminate", "GLFWSystem");
			_r->clear<GLFWWindowComponent>(); // Need to clear these before glfwTerminate is called
			glfwTerminate();
		}

		static void onMouseCaptured(entt::entity window, bool captured) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto inputMode = captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
			const auto state = captured ? "captured" : "released";

			kengine_logf(*_r, Log, "GLFWSystem", "Mouse %s for ImGui", state);
			if (captured)
				ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
			else
				ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;

			if (window == entt::null) {
				for (const auto & [e, glfw] : _r->view<GLFWWindowComponent>().each()) {
					kengine_logf(*_r, Log, "GLFWSystem", "Mouse %s for %zu", state, e);
					glfwSetInputMode(glfw.window.get(), GLFW_CURSOR, inputMode);
				}
				return;
			}

			const auto glfw = _r->try_get<GLFWWindowComponent>(window);
			if (glfw == nullptr)
				return;
			kengine_logf(*_r, Log, "GLFWSystem", "Mouse %s for %zu", state, window);
			glfwSetInputMode(glfw->window.get(), GLFW_CURSOR, inputMode);
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(*_r, Verbose, "Execute", "GLFWSystem");

			glfwPollEvents();
			for (const auto & [e, window, glfw] : _r->view<WindowComponent, GLFWWindowComponent>().each()) {
				if (glfwWindowShouldClose(glfw.window.get()))
					_r->destroy(e);
			}

			for (auto [e, window] : _r->view<WindowComponent>(entt::exclude<GLFWWindowComponent>).each()) {
				const auto initGlfw = _r->try_get<GLFWWindowInitComponent>(e);
				createWindow(e, window, initGlfw);
				_r->remove<GLFWWindowInitComponent>(e);
			}
		}

		static void createWindow(entt::entity e, WindowComponent & window, const GLFWWindowInitComponent * initGlfw) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(*_r, Log, "GLFWSystem", "Initializing window for %zu", e);
			auto & glfwComp = _r->emplace<GLFWWindowComponent>(e);

			if (initGlfw && initGlfw->setHints)
				initGlfw->setHints();

			// TODO: depend on g_windowComponent->fullscreen
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

			glfwComp.window = glfwCreateWindow((int)window.size.x, (int)window.size.y, window.name.c_str(), nullptr, nullptr);
			// Desired size may not have been available, update to actual size
			int width, height;
			glfwGetWindowSize(glfwComp.window.get(), &width, &height);
			window.size = { (unsigned int)width, (unsigned int)height };
			glfwSetWindowAspectRatio(glfwComp.window.get(), window.size.x, window.size.y);

			glfwMakeContextCurrent(glfwComp.window.get());
			glfwSetWindowSizeCallback(glfwComp.window.get(), [](GLFWwindow * window, int width, int height) noexcept {
				const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));
				auto & comp = _r->get<WindowComponent>(e);
				comp.size = { (unsigned int)width, (unsigned int)height };
			});

			glfwSetMouseButtonCallback(glfwComp.window.get(), Input::onClick);
			glfwSetCursorPosCallback(glfwComp.window.get(), Input::onMouseMove);
			glfwSetScrollCallback(glfwComp.window.get(), Input::onScroll);
			glfwSetKeyCallback(glfwComp.window.get(), Input::onKey);

			glfwSetWindowUserPointer(glfwComp.window.get(), (void *)e);

			if (initGlfw && initGlfw->onWindowCreated)
				initGlfw->onWindowCreated();
		}

		struct Input {
			static void onKey(GLFWwindow * window, int key, int scancode, int action, int mods) noexcept {
				KENGINE_PROFILING_SCOPE;

				if (_buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureKeyboard))
					return;

				const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));

				if (action == GLFW_PRESS)
					_buffer->keys.push_back(InputBufferComponent::KeyEvent{ e, key, true });
				else if (action == GLFW_RELEASE)
					_buffer->keys.push_back(InputBufferComponent::KeyEvent{ e, key, false });
			}

			static inline putils::Point2f lastPos{ FLT_MAX, FLT_MAX };

			static void onClick(GLFWwindow * window, int button, int action, int mods) noexcept {
				KENGINE_PROFILING_SCOPE;

				if (_buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse))
					return;

				const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));

				if (action == GLFW_PRESS)
					_buffer->clicks.push_back(InputBufferComponent::ClickEvent{ e, lastPos, button, true });
				else if (action == GLFW_RELEASE)
					_buffer->clicks.push_back(InputBufferComponent::ClickEvent{ e, lastPos, button, false });
			}

			static void onMouseMove(GLFWwindow * window, double xpos, double ypos) noexcept {
				KENGINE_PROFILING_SCOPE;

				if (lastPos.x == FLT_MAX) {
					lastPos.x = (float)xpos;
					lastPos.y = (float)ypos;
				}

				if (_buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse))
					return;

				const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));

				InputBufferComponent::MouseMoveEvent info;
				info.window = e;
				info.pos = { (float)xpos, (float)ypos };
				info.rel = { (float)xpos - lastPos.x, (float)ypos - lastPos.y };
				lastPos = info.pos;

				_buffer->moves.push_back(info);
			}

			static void onScroll(GLFWwindow * window, double xoffset, double yoffset) noexcept {
				KENGINE_PROFILING_SCOPE;

				if (_buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse))
					return;

				const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));
				_buffer->scrolls.push_back(InputBufferComponent::MouseScrollEvent{ e, (float)xoffset, (float)yoffset, lastPos });
			}

			static inline InputBufferComponent * _buffer;
		};

		static inline entt::registry * _r;
	};

	void GLFWSystem(entt::registry & r) noexcept {
		return GLFWSystem::init(r);
	}
}