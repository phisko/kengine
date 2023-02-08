#pragma once

// kengine functions
#include "kengine/base_function.hpp"

namespace kengine {
	enum class log_severity {
		all,
		very_verbose, // Anything that could potentially be of interest should be logged here
		verbose, // Non-essential information (potentially spam) that doesn't occur during "normal" frames (without user input)
		log, // Essential information (extra details can be added as `verbose`)
		warning, // Non-fatal issues that suggest poor usage or understanding of a tool
		error, // Issues that stop a feature from working
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
		log_severity global_severity = log_severity::log;
		std::unordered_map<std::string, log_severity> category_severities;

		bool passes(const log_event & event) noexcept {
			if (const auto it = category_severities.find(event.category); it != category_severities.end())
				return event.severity >= it->second;
			return event.severity >= global_severity;
		}
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