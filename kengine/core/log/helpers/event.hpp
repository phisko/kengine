#pragma once

// kengine
#include "kengine/core/log/helpers/severity.hpp"

namespace kengine::core::log {
	//! putils reflect all
	struct event {
		severity message_severity;
		const char * category;
		const char * message;
	};
}

#include "event.rpp"