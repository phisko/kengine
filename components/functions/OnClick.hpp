#pragma once

#include "BaseFunction.hpp"

namespace kengine::functions {
	struct OnClick : BaseFunction<
		void(int button)
	> {};
}

#define refltype kengine::functions::OnClick
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype
