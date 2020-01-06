#pragma once

#ifndef KENGINE_MAX_PYTHON_SCRIPT_PATH
# define KENGINE_MAX_PYTHON_SCRIPT_PATH 64
#endif

#ifndef KENGINE_MAX_PYTHON_SCRIPTS
# define KENGINE_MAX_PYTHON_SCRIPTS 8
#endif

#include "string.hpp"
#include "vector.hpp"

namespace kengine {
    struct PyComponent {
		static constexpr char stringName[] = "PyComponentString";
		using script = putils::string<KENGINE_MAX_PYTHON_SCRIPT_PATH, stringName>;
		static constexpr char vectorName[] = "PyComponentVector";
		using script_vector = putils::vector<script, KENGINE_MAX_PYTHON_SCRIPTS, vectorName>;

        script_vector scripts;

        putils_reflection_class_name(PyComponent);
        putils_reflection_attributes(
                putils_reflection_attribute(&PyComponent::scripts)
        );
    };
}
