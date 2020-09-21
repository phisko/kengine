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

static kengine::EntityManager * g_em;

namespace kengine {
#pragma region Input
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
#pragma endregion Input

#pragma region declarations
	static void terminate();
	static void onMouseCaptured(Entity::ID window, bool captured);
	static void execute(float deltaTime);
	static void onEntityCreated(Entity & e);
#pragma endregion
	EntityCreator * GLFWSystem(EntityManager & em) {
		g_em = &em;
		
		for (const auto & [e, buffer] : em.getEntities<InputBufferComponent>()) {
			Input::g_buffer = &buffer;
			break;
		}

		glfwInit();
		execute(0.f); // init already existing windows

		return [](Entity & e) {
			e += functions::Execute{ execute };
			e += functions::OnEntityCreated{ onEntityCreated };
			e += functions::OnTerminate{ terminate };
			e += functions::OnMouseCaptured{ onMouseCaptured };
		};
	}

#pragma region execute
#pragma region declarations
	static void init(Entity & e, WindowComponent & window);
#pragma endregion
	static void execute(float deltaTime) {
		for (const auto & [e, window, glfw] : g_em->getEntities<WindowComponent, GLFWWindowComponent>())
			if (glfwWindowShouldClose(glfw.window.get())) {
				if (window.shutdownOnClose)
					g_em->running = false;
				else
					g_em->removeEntity(e.id);
			}

		for (auto & [e, window, noGLFW] : g_em->getEntities<WindowComponent, no<GLFWWindowComponent>>())
			init(e, window);
	}

	static void init(Entity & e, WindowComponent & window) {
		auto & glfwComp = e.attach<GLFWWindowComponent>();

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
#ifndef KENGINE_NDEBUG
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
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
			auto & comp = g_em->getEntity(id).get<WindowComponent>();
			comp.size = { (unsigned int)width, (unsigned int)height };
		});

		glfwSetMouseButtonCallback(glfwComp.window.get(), Input::onClick);
		glfwSetCursorPosCallback(glfwComp.window.get(), Input::onMouseMove);
		glfwSetScrollCallback(glfwComp.window.get(), Input::onScroll);
		glfwSetKeyCallback(glfwComp.window.get(), Input::onKey);

		glfwSetWindowUserPointer(glfwComp.window.get(), (void *)e.id);
	}
#pragma endregion execute

	static void onEntityCreated(Entity & e) {
		const auto window = e.tryGet<WindowComponent>();
		if (window)
			init(e, *window);
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
			for (const auto & [e, glfw] : g_em->getEntities<GLFWWindowComponent>())
				glfwSetInputMode(glfw.window.get(), GLFW_CURSOR, inputMode);
			return;
		}

		const auto glfw = g_em->getEntity(window).tryGet<GLFWWindowComponent>();
		if (glfw == nullptr)
			return;

		glfwSetInputMode(glfw->window.get(), GLFW_CURSOR, inputMode);
	}
}