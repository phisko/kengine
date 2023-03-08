#pragma once

namespace kengine::core::log::imgui {
	//! putils reflect all
	//! class_name: core_log_imgui_config
	//! metadata: [("config", true)]
	struct config {
		int max_events = 4096;
	};
}

#include "config.rpp"