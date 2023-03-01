#include "window.hpp"

// imgui-sfml
#include <imgui-SFML.h>

namespace kengine::render::sfml {
	window::~window() noexcept {
		if (ptr)
			ImGui::SFML::Shutdown(*ptr);
	}
}