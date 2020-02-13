#pragma once

struct GLFWwindow;

namespace kengine {
	struct GLFWWindowComponent {
		GLFWwindow * window;
		putils_reflection_class_name(GLFWWindowComponent);
	};
}