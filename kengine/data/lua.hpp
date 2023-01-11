#pragma once

#ifdef KENGINE_LUA

#ifndef KENGINE_MAX_LUA_SCRIPT_PATH
#define KENGINE_MAX_LUA_SCRIPT_PATH 64
#endif

#ifndef KENGINE_MAX_LUA_SCRIPTS
#define KENGINE_MAX_LUA_SCRIPTS 8
#endif

// putils
#include "putils/string.hpp"
#include "putils/vector.hpp"

namespace kengine::data {
	//! putils reflect all
	//! used_types: [refltype::script, refltype::script_vector]
	struct lua {
		static constexpr char string_name[] = "lua_string";
		using script = putils::string<KENGINE_MAX_LUA_SCRIPT_PATH, string_name>;
		static constexpr char vector_name[] = "lua_vector";
		using script_vector = putils::vector<script, KENGINE_MAX_LUA_SCRIPTS>;

		script_vector scripts;
	};
}

#include "lua.reflection.hpp"

#endif