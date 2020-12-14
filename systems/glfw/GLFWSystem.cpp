#include <GLFW/glfw3.h>

#include "GLFWSystem.hpp"
#include "EntityManager.hpp"

#include "data/InputBufferComponent.hpp"
#include "data/GLFWWindowComponent.hpp"
#include "data/WindowComponent.hpp"

#include "functions/OnTerminate.hpp"
#include "functions/OnMouseCaptured.hpp"
#include "functions/Execute.hpp"

#include "imgui.h"
#include "with.hpp"

namespace kengine::glfw {
	namespace Input {
		static InputBufferComponent * g_buffer;

		static void onKey(GLFWwindow * window, int key, int scancode, int action, int mods) {
			if (g_buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureKeyboard))
				return;

			const auto id = (Entity::ID)glfwGetWindowUserPointer(window);

			if (action == GLFW_PRESS)
				g_buffer->keys.try_push_back(InputBufferComponent::KeyEvent{ id, key, true });
			else if (action == GLFW_RELEASE)
				g_buffer->keys.try_push_back(InputBufferComponent::KeyEvent{ id, key, false });
		}

		static putils::Point2f lastPos{ FLT_MAX, FLT_MAX };

		static void onClick(GLFWwindow * window, int button, int action, int mods) {
			if (g_buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse))
				return;

			const auto id = (Entity::ID)glfwGetWindowUserPointer(window);

			if (action == GLFW_PRESS)
				g_buffer->clicks.try_push_back(InputBufferComponent::ClickEvent{ id, lastPos, button, true });
			else if (action == GLFW_RELEASE)
				g_buffer->clicks.try_push_back(InputBufferComponent::ClickEvent{ id, lastPos, button, false });
		}

		static void onMouseMove(GLFWwindow * window, double xpos, double ypos) {
			if (lastPos.x == FLT_MAX) {
				lastPos.x = (float)xpos;
				lastPos.y = (float)ypos;
			}

			if (g_buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse))
				return;

			const auto id = (Entity::ID)glfwGetWindowUserPointer(window);

			InputBufferComponent::MouseMoveEvent info;
			info.window = id;
			info.pos = { (float)xpos, (float)ypos };
			info.rel = { (float)xpos - lastPos.x, (float)ypos - lastPos.y };
			lastPos = info.pos;

			g_buffer->moves.try_push_back(info);
		}

		static void onScroll(GLFWwindow * window, double xoffset, double yoffset) {
			if (g_buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse))
				return;

			const auto id = (Entity::ID)glfwGetWindowUserPointer(window);
			g_buffer->scrolls.try_push_back(InputBufferComponent::MouseScrollEvent{ id, (float)xoffset, (float)yoffset, lastPos });
		}
	}

	struct impl {
		static inline kengine::EntityManager * em;

		static void init(Entity & e) {
			e += functions::Execute{ glfw::impl::execute };
			e += functions::OnEntityCreated{ glfw::impl::onEntityCreated };
			e += functions::OnTerminate{ glfw::impl::terminate };
			e += functions::OnMouseCaptured{ glfw::impl::onMouseCaptured };
		
			for (const auto & [e, buffer] : em->getEntities<InputBufferComponent>()) {
				Input::g_buffer = &buffer;
				break;
			}

			glfwInit();
			execute(0.f); // init already existing windows
		}

		static void terminate() {
			glfwTerminate();
		}

		static void onMouseCaptured(Entity::ID window, bool captured) {
			const auto inputMode = captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;

			if (captured)
				ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
			else
				ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;

			if (window == Entity::INVALID_ID) {
				for (const auto & [e, glfw] : em->getEntities<GLFWWindowComponent>())
					glfwSetInputMode(glfw.window.get(), GLFW_CURSOR, inputMode);
				return;
			}

			const auto glfw = em->getEntity(window).tryGet<GLFWWindowComponent>();
			if (glfw == nullptr)
				return;

			glfwSetInputMode(glfw->window.get(), GLFW_CURSOR, inputMode);
		}

		static void onEntityCreated(Entity & e) {
			const auto window = e.tryGet<WindowComponent>();
			if (!window)
				return;

			const auto initGlfw = e.tryGet<GLFWWindowInitComponent>();
			if (!initGlfw)
				return;

			createWindow(e, *window, *initGlfw);
		}

		static void execute(float deltaTime) {
			for (const auto & [e, window, glfw] : em->getEntities<WindowComponent, GLFWWindowComponent>())
				if (glfwWindowShouldClose(glfw.window.get())) {
					if (window.shutdownOnClose)
						em->running = false;
					else
						em->removeEntity(e.id);
				}

			for (auto [e, window, initGlfw, noGLFW] : em->getEntities<WindowComponent, GLFWWindowInitComponent, no<GLFWWindowComponent>>()) {
				createWindow(e, window, initGlfw);
				e.detach<GLFWWindowInitComponent>();
			}
		}

		static void createWindow(Entity & e, WindowComponent & window, const GLFWWindowInitComponent & initGlfw) {
			auto & glfwComp = e.attach<GLFWWindowComponent>();

			if (initGlfw.setHints)
				initGlfw.setHints();

			// TODO: depend on g_windowComponent->fullscreen
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

			glfwComp.window = glfwCreateWindow((int)window.size.x, (int)window.size.y, window.name, nullptr, nullptr);
			// Desired size may not have been available, update to actual size
			int width, height;
			glfwGetWindowSize(glfwComp.window.get(), &width, &height);
			window.size = { (unsigned int)width, (unsigned int)height };
			glfwSetWindowAspectRatio(glfwComp.window.get(), window.size.x, window.size.y);

			glfwMakeContextCurrent(glfwComp.window.get());
			glfwSetWindowSizeCallback(glfwComp.window.get(), [](auto window, int width, int height) {
				const auto id = (Entity::ID)glfwGetWindowUserPointer(window);
				auto & comp = em->getEntity(id).get<WindowComponent>();
				comp.size = { (unsigned int)width, (unsigned int)height };
				});

			glfwSetMouseButtonCallback(glfwComp.window.get(), Input::onClick);
			glfwSetCursorPosCallback(glfwComp.window.get(), Input::onMouseMove);
			glfwSetScrollCallback(glfwComp.window.get(), Input::onScroll);
			glfwSetKeyCallback(glfwComp.window.get(), Input::onKey);

			glfwSetWindowUserPointer(glfwComp.window.get(), (void *)e.id);

			if (initGlfw.onWindowCreated)
				initGlfw.onWindowCreated();
		}
	};
}

namespace kengine {
	EntityCreator * GLFWSystem(EntityManager & em) {
		glfw::impl::em = &em;
		return [](Entity & e) {
			glfw::impl::init(e);
		};
	}
}