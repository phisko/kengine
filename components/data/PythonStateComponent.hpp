#pragma once

#include "python_helper.hpp"
#include "Entity.hpp"

namespace kengine {
	struct PythonStateComponent {
		struct Data {
			py::scoped_interpreter guard;
			py::module module{ "kengine" };
			py::class_<Entity> * entity;
		};

		std::unique_ptr<Data> data = nullptr;
	};
}

#define refltype kengine::PythonStateComponent
putils_reflection_info{
	putils_reflection_class_name;
};
#undef refltype