#pragma once

#ifdef KENGINE_PYTHON

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "python/python_helper.hpp"

#ifdef __GNUC__
// Ignore "declared with greater visibility than the type of its field" warnings
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wattributes"
#endif

namespace kengine {
	struct PythonStateComponent {
		struct Data {
			py::scoped_interpreter guard;
			py::module_ module_ = py::module_::create_extension_module("kengine", nullptr, new PyModuleDef);
			py::class_<entt::handle> * entity;
		};

		Data * data = nullptr;
	};
}

#ifdef __GNU_C__
#	pragma GCC diagnostic pop
#endif

#define refltype kengine::PythonStateComponent
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype

#endif