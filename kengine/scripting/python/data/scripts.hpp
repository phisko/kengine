#pragma once

#include "kengine/scripting/data/scripts.hpp"

namespace kengine::scripting::python {
	//! putils reflect all
	//! class_name: python_scripts
	//! parents: [kengine::scripting::scripts]
	struct scripts : scripting::scripts {};
}

#include "scripts.rpp"