#include "glfw_input_handler.hpp"

// imgui
#include <imgui.h>

// kengine helpers
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::systems {
	void glfw_input_handler::on_key(GLFWwindow * window, int key, int scancode, int action, int mods) noexcept {
		KENGINE_PROFILING_SCOPE;

		if (buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureKeyboard))
			return;

		const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));

		if (action == GLFW_PRESS)
			buffer->keys.push_back(data::input_buffer::key_event{ e, key, true });
		else if (action == GLFW_RELEASE)
			buffer->keys.push_back(data::input_buffer::key_event{ e, key, false });
	}

	void glfw_input_handler::on_click(GLFWwindow * window, int button, int action, int mods) noexcept {
		KENGINE_PROFILING_SCOPE;

		if (buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse))
			return;

		const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));

		if (action == GLFW_PRESS)
			buffer->clicks.push_back(data::input_buffer::click_event{ e, last_pos, button, true });
		else if (action == GLFW_RELEASE)
			buffer->clicks.push_back(data::input_buffer::click_event{ e, last_pos, button, false });
	}

	void glfw_input_handler::on_mouse_move(GLFWwindow * window, double xpos, double ypos) noexcept {
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

	void glfw_input_handler::on_scroll(GLFWwindow * window, double xoffset, double yoffset) noexcept {
		KENGINE_PROFILING_SCOPE;

		if (buffer == nullptr || (ImGui::GetCurrentContext() != nullptr && ImGui::GetIO().WantCaptureMouse))
			return;

		const auto e = entt::entity(intptr_t(glfwGetWindowUserPointer(window)));
		buffer->scrolls.push_back(data::input_buffer::mouse_scroll_event{ e, (float)xoffset, (float)yoffset, last_pos });
	}
}