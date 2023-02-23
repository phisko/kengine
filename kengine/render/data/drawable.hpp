#pragma once

// putils
#include "putils/color.hpp"

namespace kengine::render {
	//! putils reflect all
	struct drawable {
		putils::normalized_color color{ 1.f, 1.f, 1.f, 1.f };
	};
}

#include "drawable.rpp"