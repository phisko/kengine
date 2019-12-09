#pragma once

#include "BaseFunction.hpp"

namespace kengine::functions {
    struct Execute : BaseFunction<
        void(float deltaTime)
    > {
        putils_reflection_class_name(Execute);
    };
}