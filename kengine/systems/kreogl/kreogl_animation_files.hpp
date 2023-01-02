#pragma once

// kreogl
#include "kreogl/animation/animation_file.hpp"

// putils
#include "kreogl/impl/default_constructors.hpp"

namespace kengine::data {
	struct kreogl_animation_files {
		PUTILS_MOVE_ONLY(kreogl_animation_files);

		std::vector<std::unique_ptr<kreogl::animation_file>> files;
	};
}