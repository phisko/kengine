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

namespace kengine::systems {
	struct glfw {
		entt::registry & r;

		glfw(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "Init", "systems/glfw");

			e.emplace<functions::execute>(putils_forward_to_this(execute));
			e.emplace<functions::on_mouse_captured>(putils_forward_to_this(on_mouse_captured));

			for (const auto & [buffer_entity, buffer] : r.view<data::input_buffer>().each()) {
				input.buffer = &buffer;
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

			for (auto [e, window] : r.view<data::window>(entt::exclude<data::glfw_window>).each()) {
				const auto init_glfw = r.try_get<data::glfw_window_init>(e);
				create_window(e, window, init_glfw);
				r.remove<data::glfw_window_init>(e);
			}
		}

		void create_window(entt::entity e, data::window & window, const data::glfw_window_init * init_glfw) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, log, "systems/glfw", "Initializing window for %zu", e);
			auto & glfw_comp = r.emplace<data::glfw_window>(e);

			if (init_glfw && init_glfw->set_hints)
				init_glfw->set_hints();

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

#define forward_to_input(function) [](auto... args) { g_this->input.function(args...); }
			glfwSetMouseButtonCallback(glfw_comp.window.get(), forward_to_input(on_click));
			glfwSetCursorPosCallback(glfw_comp.window.get(), forward_to_input(on_mouse_move));
			glfwSetScrollCallback(glfw_comp.window.get(), forward_to_input(on_scroll));
			glfwSetKeyCallback(glfw_comp.window.get(), forward_to_input(on_key));
#undef forward_to_input

			if (init_glfw && init_glfw->on_window_created)
				init_glfw->on_window_created();
		}

		struct {
			void on_key(GLFWwindow * window, int key, int scancode, int action, int mods) noexcept {
				KENGINE_PROFILING_SCOPE;

				if (buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureKeyboard))
					return;

				const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));

				if (action == GLFW_PRESS)
					buffer->keys.push_back(data::input_buffer::key_event{ e, key, true });
				else if (action == GLFW_RELEASE)
					buffer->keys.push_back(data::input_buffer::key_event{ e, key, false });
			}

			void on_click(GLFWwindow * window, int button, int action, int mods) noexcept {
				KENGINE_PROFILING_SCOPE;

				if (buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse))
					return;

				const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));

				if (action == GLFW_PRESS)
					buffer->clicks.push_back(data::input_buffer::click_event{ e, last_pos, button, true });
				else if (action == GLFW_RELEASE)
					buffer->clicks.push_back(data::input_buffer::click_event{ e, last_pos, button, false });
			}

			void on_mouse_move(GLFWwindow * window, double xpos, double ypos) noexcept {
				KENGINE_PROFILING_SCOPE;

				if (last_pos.x == FLT_MAX) {
					last_pos.x = (float)xpos;
					last_pos.y = (float)ypos;
				}

				if (buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse))
					return;

				const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));

				data::input_buffer::mouse_move_event info;
				info.window = e;
				info.pos = { (float)xpos, (float)ypos };
				info.rel = { (float)xpos - last_pos.x, (float)ypos - last_pos.y };
				last_pos = info.pos;

				buffer->moves.push_back(info);
			}

			void on_scroll(GLFWwindow * window, double xoffset, double yoffset) noexcept {
				KENGINE_PROFILING_SCOPE;

				if (buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse))
					return;

				const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));
				buffer->scrolls.push_back(data::input_buffer::mouse_scroll_event{ e, (float)xoffset, (float)yoffset, last_pos });
			}

			putils::point2f last_pos{ FLT_MAX, FLT_MAX };
			data::input_buffer * buffer = nullptr;
		} input;
	};

	void add_glfw(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<glfw>(e);
	}
}