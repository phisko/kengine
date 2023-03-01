#pragma once

#ifndef KENGINE_SCRIPTING_MAX_SCRIPT_PATH
#define KENGINE_SCRIPTING_MAX_SCRIPT_PATH 64
#endif

#ifndef KENGINE_SCRIPTING_MAX_SCRIPTS
#define KENGINE_SCRIPTING_MAX_SCRIPTS 8
#endif

// putils
#include "putils/string.hpp"
#include "putils/vector.hpp"

namespace kengine::scripting {
	//! putils reflect all
	//! used_types: [refltype::script, refltype::script_vector]
	struct scripts {
		static constexpr char string_name[] = "scripts_string";
		using script = putils::string<KENGINE_SCRIPTING_MAX_SCRIPT_PATH, string_name>;
		static constexpr char vector_name[] = "scripts_vector";
		using script_vector = putils::vector<script, KENGINE_SCRIPTING_MAX_SCRIPTS>;

		script_vector files;
	};
}

#include "scripts.rpp"