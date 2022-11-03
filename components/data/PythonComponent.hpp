#pragma once

#ifdef KENGINE_PYTHON

#ifndef KENGINE_MAX_PYTHON_SCRIPT_PATH
# define KENGINE_MAX_PYTHON_SCRIPT_PATH 64
#endif

#ifndef KENGINE_MAX_PYTHON_SCRIPTS
# define KENGINE_MAX_PYTHON_SCRIPTS 8
#endif

// putils
#include "string.hpp"
#include "vector.hpp"

namespace kengine {
    struct PythonComponent {
		static constexpr char stringName[] = "PythonComponentString";
		using script = putils::string<KENGINE_MAX_PYTHON_SCRIPT_PATH, stringName>;
		static constexpr char vectorName[] = "PythonComponentVector";
		using script_vector = putils::vector<script, KENGINE_MAX_PYTHON_SCRIPTS, vectorName>;

        script_vector scripts;
	};
}

#define refltype kengine::PythonComponent
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