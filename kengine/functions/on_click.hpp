#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine::functions {
	struct on_click : base_function<
		void(int button)
	> {};
}

#define refltype kengine::functions::on_click
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
};
#undef refltype
