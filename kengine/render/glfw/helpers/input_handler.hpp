#pragma once

// stl
#include <limits>

// glfw
#include <GLFW/glfw3.h>

// putils
#include "putils/point.hpp"

// kengine
#include "kengine/input/data/buffer.hpp"

namespace kengine::render::glfw {
	struct input_handler {
		void on_key(GLFWwindow * window, int key, int scancode, int action, int mods) noexcept;
		void on_click(GLFWwindow * window, int button, int action, int mods) noexcept;
		void on_mouse_move(GLFWwindow * window, double xpos, double ypos) noexcept;
		void on_scroll(GLFWwindow * window, double xoffset, double yoffset) noexcept;

		putils::point2f last_pos{ std::numeric_limits<float>::max(), std::numeric_limits<float>::max() };
		input::buffer * buffer = nullptr;
	};
}