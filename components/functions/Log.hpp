#pragma once

#include "BaseFunction.hpp"

namespace kengine::functions {
    struct Log : BaseFunction<
        void(const char * msg)
    > {
        putils_reflection_class_name(Log);
    };
}