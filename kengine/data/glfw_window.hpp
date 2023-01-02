#pragma once

// glfw
#include <GLFW/glfw3.h>

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/raii.hpp"

namespace kengine::data {
	struct glfw_window {
		static inline void free_window(GLFWwindow *(&window)) {
			if (window)
				glfwDestroyWindow(window);
		}

		putils::raii<GLFWwindow *, free_window> window{ nullptr };
	};

	struct glfw_window_init {
		std::function<void()> set_hints = nullptr;
		std::function<void()> on_window_created = nullptr;
	};
}

#define refltype kengine::data::glfw_window
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype