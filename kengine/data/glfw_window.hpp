#pragma once

#ifdef KENGINE_GLFW

// glfw
#include <GLFW/glfw3.h>

// putils
#include "putils/raii.hpp"

namespace kengine::data {
	//! putils reflect none
	struct glfw_window {
		static inline void free_window(GLFWwindow *(&window)) {
			if (window)
				glfwDestroyWindow(window);
		}

		putils::raii<GLFWwindow *, free_window> window{ nullptr };
	};

	//! putils reflect all
	struct glfw_window_init {
		std::function<void()> set_hints = nullptr;
		std::function<void()> on_window_created = nullptr;
	};
}

#include "glfw_window.reflection.hpp"

#endif