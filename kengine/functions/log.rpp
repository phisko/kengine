#pragma once

#include "putils/reflection.hpp"

#define refltype kengine::log_event
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(severity),
		putils_reflection_attribute(category),
		putils_reflection_attribute(message)
	);
};
#undef refltype

#define refltype kengine::log_severity_control
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(severity)
	);
};
#undef refltype

#define refltype kengine::functions::log
putils_reflection_info {
	putils_reflection_class_name;
	putils_reflection_parents(
		putils_reflection_type(refltype::base)
	);
	putils_reflection_used_types(
		putils_reflection_type(kengine::log_event)
	);
};
#undef refltype