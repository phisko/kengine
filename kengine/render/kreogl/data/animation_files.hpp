#pragma once

// kreogl
#include "kreogl/animation/animation_file.hpp"

// putils
#include "putils/default_constructors.hpp"

namespace kengine::render::kreogl {
	//! putils reflect name
	//! class_name: kreogl_animation_files
	struct animation_files {
		PUTILS_MOVE_ONLY(animation_files);

		std::vector<std::unique_ptr<::kreogl::animation_file>> files;
	};
}

#include "animation_files.rpp"