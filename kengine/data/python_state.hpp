#pragma once

#ifdef KENGINE_PYTHON

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "putils/default_constructors.hpp"
#include "putils/python/python_helper.hpp"

#ifdef __GNUC__
// Ignore "declared with greater visibility than the type of its field" warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#endif

namespace kengine::data {
	struct python_state {
		PUTILS_MOVE_ONLY(python_state);

		py::scoped_interpreter guard;
		py::module_ module_ = py::module_::create_extension_module("kengine", nullptr, new PyModuleDef);
		py::class_<entt::handle> * entity;
	};
}

#ifdef __GNU_C__
#pragma GCC diagnostic pop
#endif

#define refltype kengine::data::python_state
putils_reflection_info {
	putils_reflection_class_name;
};
#undef refltype

#endif