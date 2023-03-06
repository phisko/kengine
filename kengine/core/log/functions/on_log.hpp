#pragma once

// kengine
#include "kengine/base_function.hpp"
#include "kengine/core/log/helpers/event.hpp"

namespace kengine::core::log {
	using log_signature = void(const event & log);
	//! putils reflect all
	//! parents: [refltype::base]
	//! used_types: [kengine::core::log::event]
	struct on_log : base_function<log_signature> {};
}

#include "on_log.rpp"