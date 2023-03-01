#pragma once

// putils
#include "putils/point.hpp"

// kengine
#include "kengine/render/data/camera.hpp"

namespace kengine::render {
	struct facings {
		putils::vec3f front;
		putils::vec3f right;
		putils::vec3f up;
	};

	KENGINE_RENDER_EXPORT facings get_facings(const camera & camera) noexcept;
}