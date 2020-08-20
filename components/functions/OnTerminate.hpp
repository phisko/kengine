#pragma once

#include "BaseFunction.hpp"

namespace kengine::functions {
    struct OnTerminate : BaseFunction<
        void()
    > {};
}

#define refltype kengine::functions::OnTerminate
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype