#include "log.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine core/profiling
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine core/log
#include "kengine/core/log/data/severity_control.hpp"

namespace kengine::core::log {
	void log(const entt::registry & r, severity message_severity, const char * category, const char * message) noexcept {
		KENGINE_PROFILING_SCOPE;

		const event log_event{
			.message_severity = message_severity,
			.category = category,
			.message = message
		};

		for (const auto & [e, log] : r.view<on_log>().each()) {
			if (const auto control = r.try_get<severity_control>(e))
				if (!control->passes(log_event))
					continue;
			log(log_event);
		}
	}
}