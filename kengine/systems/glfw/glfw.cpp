#include "glfw.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// glfw
#include <GLFW/glfw3.h>

// imgui
#include <imgui.h>

// putils
#include "putils/forward_to.hpp"

// kengine data
#include "kengine/data/glfw_window.hpp"
#include "kengine/data/input_buffer.hpp"
#include "kengine/data/window.hpp"

// kengine functions
#include "kengine/functions/on_mouse_captured.hpp"
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/assert_helper.hpp"
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

// impl
#include "glfw_input_handler.hpp"

namespace kengine::systems {
	struct glfw {
		entt::registry & r;
		glfw_input_handler input_handler;

		glfw(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "Init", "systems/glfw");

			e.emplace<functions::execute>(putils_forward_to_this(execute));
			e.emplace<functions::on_mouse_captured>(putils_forward_to_this(on_mouse_captured));

			for (const auto & [buffer_entity, buffer] : r.view<data::input_buffer>().each()) {
				input_handler.buffer = &buffer;
				break;
			}

			static bool is_glfw_init = false;
			if (!is_glfw_init) {
				is_glfw_init = true;

				static const entt::registry * g_r = &r;
				glfwSetErrorCallback([](int error, const char * desc) {
					kengine_logf(*g_r, error, "GLFW", "Error code: %d. Description: '%s'", error, desc);
				});

				const auto ret = glfwInit();
				kengine_assert(r, ret == GLFW_TRUE);
				execute(0.f); // init already existing windows
			}
		}

		~glfw() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "Terminate", "systems/glfw");
			r.clear<data::glfw_window>(); // Need to clear these before glfwTerminate is called
			glfwTerminate();
		}

		void on_mouse_captured(entt::entity window, bool captured) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto input_mode = captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
			const auto state = captured ? "captured" : "released";

			kengine_logf(r, log, "systems/glfw", "Mouse %s for ImGui", state);
			if (captured)
				ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
			else
				ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;

			if (window == entt::null) {
				for (const auto & [e, glfw] : r.view<data::glfw_window>().each()) {
					kengine_logf(r, log, "systems/glfw", "Mouse %s for %zu", state, e);
					glfwSetInputMode(glfw.window.get(), GLFW_CURSOR, input_mode);
				}
				return;
			}

			const auto glfw = r.try_get<data::glfw_window>(window);
			if (glfw == nullptr)
				return;
			kengine_logf(r, log, "systems/glfw", "Mouse %s for %zu", state, window);
			glfwSetInputMode(glfw->window.get(), GLFW_CURSOR, input_mode);
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, "execute", "systems/glfw");

			glfwPollEvents();
			for (const auto & [e, window, glfw] : r.view<data::window, data::glfw_window>().each()) {
				if (glfwWindowShouldClose(glfw.window.get()))
					r.destroy(e);
			}

			for (auto [e, window, init_glfw] : r.view<data::window, data::glfw_window_init>().each()) {
				create_window(e, window, init_glfw);
				r.remove<data::glfw_window_init>(e);
			}
		}

		void create_window(entt::entity e, data::window & window, const data::glfw_window_init & init_glfw) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, log, "systems/glfw", "Initializing window for %zu", e);
			auto & glfw_comp = r.emplace<data::glfw_window>(e);

			if (init_glfw.set_hints)
				init_glfw.set_hints();

			// TODO: depend on g_windowComponent->fullscreen
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

			glfw_comp.window = glfwCreateWindow((int)window.size.x, (int)window.size.y, window.name.c_str(), nullptr, nullptr);
			// Desired size may not have been available, update to actual size
			int width, height;
			glfwGetWindowSize(glfw_comp.window.get(), &width, &height);
			window.size = { (unsigned int)width, (unsigned int)height };
			glfwSetWindowAspectRatio(glfw_comp.window.get(), window.size.x, window.size.y);

			glfwMakeContextCurrent(glfw_comp.window.get());

			static glfw * g_this = nullptr;
			kengine_assert_with_message(r, !g_this, "glfw system doesn't support existing in multiple registries currently. Fix this!");
			g_this = this;

			glfwSetWindowUserPointer(glfw_comp.window.get(), (void *)e);
			glfwSetWindowSizeCallback(glfw_comp.window.get(), [](GLFWwindow * window, int width, int height) noexcept {
				const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));
				auto & comp = g_this->r.get<data::window>(e);
				comp.size = { (unsigned int)width, (unsigned int)height };
			});

#define forward_to_input_handler(function) [](auto... args) { g_this->input_handler.function(args...); }
			glfwSetMouseButtonCallback(glfw_comp.window.get(), forward_to_input_handler(on_click));
			glfwSetCursorPosCallback(glfw_comp.window.get(), forward_to_input_handler(on_mouse_move));
			glfwSetScrollCallback(glfw_comp.window.get(), forward_to_input_handler(on_scroll));
			glfwSetKeyCallback(glfw_comp.window.get(), forward_to_input_handler(on_key));
#undef forward_to_input_handler

			if (init_glfw.on_window_created)
				init_glfw.on_window_created();
		}

	};

	DEFINE_KENGINE_SYSTEM_CREATOR(glfw)
}