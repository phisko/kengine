#pragma once

// kreogl
#include "kreogl/animation/animated_model.hpp"

namespace kengine::data {
	struct kreogl_model {
		std::unique_ptr<kreogl::animated_model> model;
	};
}