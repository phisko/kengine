#pragma once

// glfw
#include <GLFW/glfw3.h>

// putils
#include "putils/raii.hpp"

namespace kengine::render::glfw {
	//! putils reflect name
	//! class_name: glfw_window
	struct window {
		static inline void free_window(GLFWwindow *(&window)) {
			if (window)
				glfwDestroyWindow(window);
		}

		putils::raii<GLFWwindow *, free_window> ptr{ nullptr };
	};

	//! putils reflect all
	//! class_name: glfw_window_init
	struct window_init {
		std::function<void()> set_hints = nullptr;
		std::function<void()> on_window_created = nullptr;
	};
}

#include "window.rpp"