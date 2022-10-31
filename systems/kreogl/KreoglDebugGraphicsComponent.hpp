#pragma once

#include <vector>
#include "kreogl/DebugElement.hpp"

namespace kengine {
	struct KreoglDebugGraphicsComponent {
		std::vector<kreogl::DebugElement> elements;
	};
}