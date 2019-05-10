#pragma once

namespace kengine::packets {
	// Adds a `name` entry in the ImGui "Tools" menu bar, which when clicked will toggle `enabled`
	struct AddImGuiTool {
		const char * name;
		bool & enabled;
	};
}