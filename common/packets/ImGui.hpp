#pragma once

#include <functional>
#include "reflection/Reflectible.hpp"

struct ImGuiContext;
extern ImGuiContext * GImGui;

namespace kengine {
	namespace packets {
		struct ImGuiDisplay {
			std::function<void(ImGuiContext & context)> display;

			static void setupImGuiContext(ImGuiContext & context) {
				GImGui = &context;
			}

			/*
			 * Reflectible
			 */

            pmeta_get_class_name(ImGuiDisplay);
            pmeta_get_attributes(
                    pmeta_reflectible_attribute(&ImGuiDisplay::display)
            );
            pmeta_get_methods();
            pmeta_get_parents();
		};
	}
}