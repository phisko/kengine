#pragma once

// kreogl
#include "kreogl/animation/AnimationFile.hpp"

// putils
#include "default_constructors.hpp"

namespace kengine {
    struct KreoglAnimationFilesComponent {
		PUTILS_MOVE_ONLY(KreoglAnimationFilesComponent);

        std::vector<std::unique_ptr<kreogl::AnimationFile>> files;
    };
}