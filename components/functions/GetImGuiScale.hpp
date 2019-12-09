#pragma once

#include "BaseFunction.hpp"

namespace kengine::functions {
    struct GetImGuiScale : BaseFunction<
        float()
    > {
        putils_reflection_class_name(GetImGuiScale);
    };
}