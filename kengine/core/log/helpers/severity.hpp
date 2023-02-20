#pragma once

namespace kengine::core::log {
	enum class severity {
		all,
		very_verbose, // Anything that could potentially be of interest should be logged here
		verbose, // Non-essential information (potentially spam) that doesn't occur during "normal" frames (without user input)
		log, // Essential information (extra details can be added as `verbose`)
		warning, // Non-fatal issues that suggest poor usage or understanding of a tool
		error, // Issues that stop a feature from working
		none
	};
}