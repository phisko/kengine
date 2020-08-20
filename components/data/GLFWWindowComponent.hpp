#pragma once

struct GLFWwindow;

namespace kengine {
	struct GLFWWindowComponent {
		GLFWwindow * window;
	};
}

#define refltype kengine::GLFWWindowComponent
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype