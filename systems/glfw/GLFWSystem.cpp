#include "GLFWSystem.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// glfw
#include <GLFW/glfw3.h>

// imgui
#include <imgui.h>

// putils
#include "forward_to.hpp"

// kengine data
#include "data/GLFWWindowComponent.hpp"
#include "data/InputBufferComponent.hpp"
#include "data/WindowComponent.hpp"

// kengine functions
#include "functions/OnMouseCaptured.hpp"
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/assertHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	struct GLFWSystem {
		entt::registry & r;

		GLFWSystem(entt::handle e) noexcept
			: r(*e.registry())
		{
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "GLFWSystem");

			e.emplace<functions::Execute>(putils_forward_to_this(execute));
			e.emplace<functions::OnMouseCaptured>(putils_forward_to_this(onMouseCaptured));

			for (const auto & [bufferEntity, buffer] : r.view<InputBufferComponent>().each()) {
				input.buffer = &buffer;
				break;
			}

			static bool isGlfwInit = false;
			if (!isGlfwInit) {
				isGlfwInit = true;

				static const entt::registry * g_r = &r;
				glfwSetErrorCallback([](int error, const char * desc) {
					kengine_logf(*g_r, Error, "GLFW", "Error code: %d. Description: '%s'", error, desc);
				});

				const auto ret = glfwInit();
				kengine_assert(r, ret == GLFW_TRUE);
				execute(0.f); // init already existing windows
			}
		}

		~GLFWSystem() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Terminate", "GLFWSystem");
			r.clear<GLFWWindowComponent>(); // Need to clear these before glfwTerminate is called
			glfwTerminate();
		}

		void onMouseCaptured(entt::entity window, bool captured) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto inputMode = captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
			const auto state = captured ? "captured" : "released";

			kengine_logf(r, Log, "GLFWSystem", "Mouse %s for ImGui", state);
			if (captured)
				ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
			else
				ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;

			if (window == entt::null) {
				for (const auto & [e, glfw] : r.view<GLFWWindowComponent>().each()) {
					kengine_logf(r, Log, "GLFWSystem", "Mouse %s for %zu", state, e);
					glfwSetInputMode(glfw.window.get(), GLFW_CURSOR, inputMode);
				}
				return;
			}

			const auto glfw = r.try_get<GLFWWindowComponent>(window);
			if (glfw == nullptr)
				return;
			kengine_logf(r, Log, "GLFWSystem", "Mouse %s for %zu", state, window);
			glfwSetInputMode(glfw->window.get(), GLFW_CURSOR, inputMode);
		}

		void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Verbose, "Execute", "GLFWSystem");

			glfwPollEvents();
			for (const auto & [e, window, glfw] : r.view<WindowComponent, GLFWWindowComponent>().each()) {
				if (glfwWindowShouldClose(glfw.window.get()))
					r.destroy(e);
			}

			for (auto [e, window] : r.view<WindowComponent>(entt::exclude<GLFWWindowComponent>).each()) {
				const auto initGlfw = r.try_get<GLFWWindowInitComponent>(e);
				createWindow(e, window, initGlfw);
				r.remove<GLFWWindowInitComponent>(e);
			}
		}

		void createWindow(entt::entity e, WindowComponent & window, const GLFWWindowInitComponent * initGlfw) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, Log, "GLFWSystem", "Initializing window for %zu", e);
			auto & glfwComp = r.emplace<GLFWWindowComponent>(e);

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

			static GLFWSystem * g_this = nullptr;
			kengine_assert_with_message(r, !g_this, "GLFWSystem doesn't support existing in multiple registries currently. Fix this!");
			g_this = this;

			glfwSetWindowUserPointer(glfwComp.window.get(), (void *)e);
			glfwSetWindowSizeCallback(glfwComp.window.get(), [](GLFWwindow * window, int width, int height) noexcept {
				const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));
				auto & comp = g_this->r.get<WindowComponent>(e);
				comp.size = {(unsigned int) width, (unsigned int) height};
			});

#define forward_to_input(function) [](auto ... args) noexcept { g_this->input.function(args...); }
			glfwSetMouseButtonCallback(glfwComp.window.get(), forward_to_input(onClick));
			glfwSetCursorPosCallback(glfwComp.window.get(), forward_to_input(onMouseMove));
			glfwSetScrollCallback(glfwComp.window.get(), forward_to_input(onScroll));
			glfwSetKeyCallback(glfwComp.window.get(), forward_to_input(onKey));
#undef forward_to_input

			if (initGlfw && initGlfw->onWindowCreated)
				initGlfw->onWindowCreated();
		}

		struct {
			void onKey(GLFWwindow * window, int key, int scancode, int action, int mods) noexcept {
				KENGINE_PROFILING_SCOPE;

				if (buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureKeyboard))
					return;

				const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));

				if (action == GLFW_PRESS)
					buffer->keys.push_back(InputBufferComponent::KeyEvent{ e, key, true });
				else if (action == GLFW_RELEASE)
					buffer->keys.push_back(InputBufferComponent::KeyEvent{ e, key, false });
			}

			void onClick(GLFWwindow * window, int button, int action, int mods) noexcept {
				KENGINE_PROFILING_SCOPE;

				if (buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse))
					return;

				const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));

				if (action == GLFW_PRESS)
					buffer->clicks.push_back(InputBufferComponent::ClickEvent{ e, lastPos, button, true });
				else if (action == GLFW_RELEASE)
					buffer->clicks.push_back(InputBufferComponent::ClickEvent{ e, lastPos, button, false });
			}

			void onMouseMove(GLFWwindow * window, double xpos, double ypos) noexcept {
				KENGINE_PROFILING_SCOPE;

				if (lastPos.x == FLT_MAX) {
					lastPos.x = (float)xpos;
					lastPos.y = (float)ypos;
				}

				if (buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse))
					return;

				const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));

				InputBufferComponent::MouseMoveEvent info;
				info.window = e;
				info.pos = { (float)xpos, (float)ypos };
				info.rel = { (float)xpos - lastPos.x, (float)ypos - lastPos.y };
				lastPos = info.pos;

				buffer->moves.push_back(info);
			}

			void onScroll(GLFWwindow * window, double xoffset, double yoffset) noexcept {
				KENGINE_PROFILING_SCOPE;

				if (buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse))
					return;

				const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));
				buffer->scrolls.push_back(InputBufferComponent::MouseScrollEvent{ e, (float)xoffset, (float)yoffset, lastPos });
			}

			putils::Point2f lastPos{ FLT_MAX, FLT_MAX };
			InputBufferComponent * buffer = nullptr;
		} input;
	};

	void addGLFWSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<GLFWSystem>(e);
	}
}