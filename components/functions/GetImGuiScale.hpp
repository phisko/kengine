#pragma once

#include "BaseFunction.hpp"

namespace kengine::functions {
    struct GetImGuiScale : BaseFunction<
        float()
    > {};
}

#define refltype kengine::functions::GetImGuiScale
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype