#pragma once

// entt
#include <entt/entity/fwd.hpp>

// putils
#include "putils/python/python_helper.hpp"

#ifdef __GNUC__
// Ignore "declared with greater visibility than the type of its field" warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
#endif

namespace kengine::scripting::python {
	//! putils reflect none
	//! class_name: python_state
	struct state {
		state() noexcept = default;

		py::scoped_interpreter guard;
		py::module_ module_ = py::module_::create_extension_module("kengine", nullptr, new PyModuleDef);
		py::class_<entt::handle> * entity;
	};
}

#ifdef __GNU_C__
#pragma GCC diagnostic pop
#endif

#include "state.rpp"