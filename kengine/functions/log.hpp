#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine {
	enum class log_severity {
		all,
		verbose,
		log,
		warning,
		error,
		none
	};

	struct log_event {
		log_severity severity;
		const char * category;
		const char * message;
	};

	struct log_severity_control {
		log_severity severity = log_severity::log;
	};

	namespace functions {
		using log_signature = void(const log_event & log);
		struct log : base_function<log_signature> {};
	}
}

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
