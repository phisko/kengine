#pragma once

// kreogl
#include "kreogl/animation/animated_model.hpp"

namespace kengine::render::kreogl {
	//! putils reflect name
	//! class_name: kreogl_model
	struct model {
		std::unique_ptr<::kreogl::animated_model> ptr;
	};
}

#include "model.rpp"