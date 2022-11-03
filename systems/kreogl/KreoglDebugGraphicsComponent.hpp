#pragma once

// stl
#include <vector>

// kreogl
#include "kreogl/DebugElement.hpp"

namespace kengine {
	struct KreoglDebugGraphicsComponent {
		std::vector<kreogl::DebugElement> elements;
	};
}