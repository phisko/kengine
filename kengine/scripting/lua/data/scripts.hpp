#pragma once

// kengine
#include "kengine/scripting/data/scripts.hpp"

namespace kengine::scripting::lua {
	//! putils reflect all
	//! class_name: lua_scripts
	//! parents: [kengine::scripting::scripts]
	struct scripts : scripting::scripts {};
}

#include "scripts.rpp"