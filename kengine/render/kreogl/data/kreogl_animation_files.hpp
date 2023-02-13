#pragma once

// kreogl
#include "kreogl/animation/animation_file.hpp"

// putils
#include "putils/default_constructors.hpp"

namespace kengine::data {
	//! putils reflect name
	struct kreogl_animation_files {
		PUTILS_MOVE_ONLY(kreogl_animation_files);

		std::vector<std::unique_ptr<kreogl::animation_file>> files;
	};
}

#include "kreogl_animation_files.rpp"