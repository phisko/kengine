#pragma once

#include "BaseFunction.hpp"

namespace kengine::functions {
    struct Execute : BaseFunction<
        void(float deltaTime)
    > {};
}

#define refltype kengine::functions::Execute
putils_reflection_info{
    putils_reflection_class_name;
};
#undef refltype