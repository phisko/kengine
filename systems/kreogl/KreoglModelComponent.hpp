#pragma once

// kreogl
#include "kreogl/animation/AnimatedModel.hpp"

namespace kengine {
    struct KreoglModelComponent {
        std::unique_ptr<kreogl::AnimatedModel> model;
    };
}