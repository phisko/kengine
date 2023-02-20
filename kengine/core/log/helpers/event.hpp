#pragma once

// kengine core/log
#include "kengine/core/log/helpers/severity.hpp"

namespace kengine::core::log {
	//! putils reflect all
	struct event {
		severity severity;
		const char * category;
		const char * message;
	};
}

#include "event.rpp"