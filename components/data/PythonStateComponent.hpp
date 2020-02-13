#pragma once

#include "python.hpp"
#include "Entity.hpp"

namespace kengine {
	struct PythonStateComponent {
		struct Data {
			py::scoped_interpreter guard;
			py::module module{ "pk" };
			py::class_<Entity> * entity;
		};

		std::unique_ptr<Data> data = nullptr;
		putils_reflection_class_name(PythonStateComponent);
	};
}