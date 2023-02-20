#pragma once

// stl
#include <unordered_map>

// kengine core/log
#include "kengine/core/log/helpers/event.hpp"
#include "kengine/core/log/helpers/severity.hpp"

namespace kengine::core::log {
	//! putils reflect all
	//! used_types: [kengine::core::log::event]
	struct severity_control {
		severity global_severity = severity::log;
		std::unordered_map<std::string, severity> category_severities;

		bool passes(const event & event) const noexcept {
			if (const auto it = category_severities.find(event.category); it != category_severities.end())
				return event.severity >= it->second;
			return event.severity >= global_severity;
		}
	};
}

#include "severity_control.rpp"