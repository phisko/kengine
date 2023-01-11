#pragma once

// putils
#include "putils/point.hpp"
#include "putils/color.hpp"

namespace kengine::data {
	//! putils reflect all
	//! used_types: [putils::normalized_color]
	struct highlight {
		putils::normalized_color color;
		float intensity = 1.f;
	};
}

#include "highlight.reflection.hpp"