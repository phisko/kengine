#pragma once

#include "BaseFunction.hpp"

namespace kengine::functions {
    struct OnTerminate : BaseFunction<
        void()
    > {
        putils_reflection_class_name(OnTerminate);
    };
}