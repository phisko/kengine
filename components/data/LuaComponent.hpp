#pragma once

#ifndef KENGINE_MAX_LUA_SCRIPT_PATH
# define KENGINE_MAX_LUA_SCRIPT_PATH 64
#endif

#ifndef KENGINE_MAX_LUA_SCRIPTS
# define KENGINE_MAX_LUA_SCRIPTS 8
#endif

#include "string.hpp"
#include "vector.hpp"
#include "reflection.hpp"

namespace kengine {
    struct LuaComponent {
		static constexpr char stringName[] = "LuaComponentString";
		using script = putils::string<KENGINE_MAX_LUA_SCRIPT_PATH, stringName>;
		static constexpr char vectorName[] = "LuaComponentVector";
		using script_vector = putils::vector<script, KENGINE_MAX_LUA_SCRIPTS>;

        script_vector scripts;
	};
}

#define refltype kengine::LuaComponent
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(scripts)
	);
	putils_reflection_used_types(
		putils_reflection_type(refltype::script),
		putils_reflection_type(refltype::script_vector)
	);
};
#undef refltype