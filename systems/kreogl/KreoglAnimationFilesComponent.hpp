#pragma once

#include "kreogl/animation/AnimationFile.hpp"

namespace kengine {
    struct KreoglAnimationFilesComponent {
        std::vector<std::unique_ptr<kreogl::AnimationFile>> files;
    };
}