#include "system.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// glfw
#include <GLFW/glfw3.h>

// imgui
#include <imgui.h>

// putils
#include "putils/forward_to.hpp"

// kengine core
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/helpers/new_entity_processor.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine imgui
#include "kengine/imgui/helpers/set_context.hpp"

// kengine input
#include "kengine/input/data/buffer.hpp"

// kengine main_loop
#include "kengine/main_loop/functions/execute.hpp"

// kengine render
#include "kengine/render/data/window.hpp"
#include "kengine/render/functions/on_mouse_captured.hpp"

// kengine render/glfw
#include "kengine/render/glfw/data/window.hpp"
#include "kengine/render/glfw/helpers/input_handler.hpp"

namespace kengine::render::glfw {
	static constexpr auto log_category = "render_glfw";

	struct system {
		entt::registry & r;
		input_handler input;

		struct processed_input_buffer {};
		kengine::new_entity_processor<processed_input_buffer, input::buffer> input_buffer_processor{ r, putils_forward_to_this(set_input_buffer) };

		struct processed_window {};
		kengine::new_entity_processor<processed_window, render::window, window_init> window_processor{ r, putils_forward_to_this(create_window) };

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));
			e.emplace<render::on_mouse_captured>(putils_forward_to_this(on_mouse_captured));

			input_buffer_processor.process();
			window_processor.process();
		}

		~system() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Shutting down");
			r.clear<window>(); // Need to clear these before glfwTerminate is called
			glfwTerminate();
		}

		void on_mouse_captured(entt::entity window, bool captured) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto input_mode = captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL;
			const auto state = captured ? "captured" : "released";

			if (imgui::set_context(r)) {
				kengine_logf(r, verbose, log_category, "Mouse {} for ImGui", state);
				if (captured)
					ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
				else
					ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
			}

			if (window == entt::null) {
				for (const auto & [e, glfw] : r.view<glfw::window>().each()) {
					kengine_logf(r, verbose, log_category, "Mouse {} for {}", state, e);
					glfwSetInputMode(glfw.ptr.get(), GLFW_CURSOR, input_mode);
				}
				return;
			}

			const auto glfw = r.try_get<glfw::window>(window);
			if (glfw == nullptr)
				return;
			kengine_logf(r, verbose, log_category, "Mouse {} for {}", state, window);
			glfwSetInputMode(glfw->ptr.get(), GLFW_CURSOR, input_mode);
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");

			input_buffer_processor.process();
			window_processor.process();

			kengine_log(r, very_verbose, log_category, "Polling events");
			glfwPollEvents();

			kengine_log(r, very_verbose, log_category, "Checking if windows were closed");
			for (const auto & [e, window, glfw] : r.view<render::window, window>().each()) {
				kengine_logf(r, very_verbose, log_category, "Checking if {} was closed", e);
				if (glfwWindowShouldClose(glfw.ptr.get())) {
					kengine_logf(r, log, log_category, "Destroying {} because its window was closed", e);
					r.destroy(e);
				}
			}
		}

		void set_input_buffer(entt::entity e, input::buffer & input_buffer) noexcept {
			kengine_log(r, verbose, log_category, "Setting input buffer");
			kengine_assert(r, input.buffer == nullptr);
			input.buffer = &input_buffer;
		}

		void create_window(entt::entity e, render::window & window, const window_init & init_glfw) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Creating window for {}", e);

			init_global_glfw();

			auto & glfw_comp = r.emplace<glfw::window>(e);

			if (init_glfw.set_hints)
				init_glfw.set_hints();

			// TODO: depend on g_windowComponent->fullscreen
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

			glfw_comp.ptr = glfwCreateWindow((int)window.size.x, (int)window.size.y, window.name.c_str(), nullptr, nullptr);
			// Desired size may not have been available, update to actual size
			int width, height;
			glfwGetWindowSize(glfw_comp.ptr.get(), &width, &height);
			window.size = { (unsigned int)width, (unsigned int)height };
			glfwSetWindowAspectRatio(glfw_comp.ptr.get(), window.size.x, window.size.y);

			glfwMakeContextCurrent(glfw_comp.ptr.get());

			static system * g_this = nullptr;
			kengine_assert_with_message(r, !g_this, "glfw system doesn't support existing in multiple registries currently. Fix this!");
			g_this = this;

			glfwSetWindowUserPointer(glfw_comp.ptr.get(), (void *)e);
			glfwSetWindowSizeCallback(glfw_comp.ptr.get(), [](GLFWwindow * window, int width, int height) noexcept {
				const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));
				kengine_logf(g_this->r, verbose, log_category, "Window size for {} changed to ({}, {})", e, width, height);
				auto & comp = g_this->r.get<render::window>(e);
				comp.size = { (unsigned int)width, (unsigned int)height };
			});

#define forward_to_input_handler(function) [](auto... args) { g_this->input.function(args...); }
			glfwSetMouseButtonCallback(glfw_comp.ptr.get(), forward_to_input_handler(on_click));
			glfwSetCursorPosCallback(glfw_comp.ptr.get(), forward_to_input_handler(on_mouse_move));
			glfwSetScrollCallback(glfw_comp.ptr.get(), forward_to_input_handler(on_scroll));
			glfwSetKeyCallback(glfw_comp.ptr.get(), forward_to_input_handler(on_key));
#undef forward_to_input_handler

			if (init_glfw.on_window_created) {
				kengine_log(r, verbose, log_category, "Calling on_window_created");
				init_glfw.on_window_created();
			}
		}

		void init_global_glfw() const noexcept {
			KENGINE_PROFILING_SCOPE;

			static bool is_glfw_init = false;
			if (!is_glfw_init) {
				is_glfw_init = true;

				kengine_log(r, log, log_category, "Performing one-time GLFW initialization");

				static const entt::registry * g_r = &r;
				glfwSetErrorCallback([](int error, const char * desc) {
					kengine_logf(*g_r, error, log_category, "Error code: {}. Description: '{}'", error, desc);
				});

				const auto ret = glfwInit();
				kengine_assert(r, ret == GLFW_TRUE);
			}
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		system,
		system::processed_input_buffer,
		system::processed_window
	)
}