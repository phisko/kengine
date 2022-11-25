#pragma once

#ifdef KENGINE_PYTHON

#ifndef KENGINE_MAX_PYTHON_SCRIPT_PATH
# define KENGINE_MAX_PYTHON_SCRIPT_PATH 64
#endif

#ifndef KENGINE_MAX_PYTHON_SCRIPTS
# define KENGINE_MAX_PYTHON_SCRIPTS 8
#endif

// putils
#include "putils/string.hpp"
#include "putils/vector.hpp"

namespace kengine::data {
    struct python {
		static constexpr char string_name[] = "python_string";
		using script = putils::string<KENGINE_MAX_PYTHON_SCRIPT_PATH, string_name>;
		static constexpr char vector_name[] = "python_vector";
		using script_vector = putils::vector<script, KENGINE_MAX_PYTHON_SCRIPTS, vector_name>;

        script_vector scripts;
	};
}

#define refltype kengine::data::python
putils_reflection_info{
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

#endif