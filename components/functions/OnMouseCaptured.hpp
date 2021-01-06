#pragma once

#include "BaseFunction.hpp"
#include "Entity.hpp"

namespace kengine::functions {
    struct OnMouseCaptured : BaseFunction<
        void(EntityID window, bool captured)
    > {};
}

#define refltype kengine::functions::OnMouseCaptured
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype