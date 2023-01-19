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

	//! putils reflect all
	struct log_event {
		log_severity severity;
		const char * category;
		const char * message;
	};

	//! putils reflect all
	struct log_severity_control {
		log_severity severity = log_severity::log;
	};

	namespace functions {
		using log_signature = void(const log_event & log);
		//! putils reflect all
		//! parents: [refltype::base]
		//! used_types: [kengine::log_event]
		struct log : base_function<log_signature> {};
	}
}

#include "log.rpp"