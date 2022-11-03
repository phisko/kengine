#pragma once

// glfw
#include <GLFW/glfw3.h>

// putils
#include "RAII.hpp"

namespace kengine {
	struct GLFWWindowComponent {
		static inline void freeWindow(GLFWwindow * (&window)) {
			if (window)
				glfwDestroyWindow(window);
		}

		putils::RAII<GLFWwindow *, freeWindow> window{ nullptr };
	};

	struct GLFWWindowInitComponent {
		std::function<void()> setHints = nullptr;
		std::function<void()> onWindowCreated = nullptr;
	};
}

#define refltype kengine::GLFWWindowComponent
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype