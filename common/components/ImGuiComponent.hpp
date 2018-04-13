#pragma once

#include <functional>
#include "SerializableComponent.hpp"

struct ImGuiContext;
extern ImGuiContext * GImGui;

namespace kengine {
	class ImGuiComponent : public kengine::SerializableComponent<ImGuiComponent> {
	public:
		ImGuiComponent(const std::function<void(void *)> & display = nullptr) : display(display) {}

	public:
		std::function<void(void * context)> display;

		static void setupImGuiContext(void * context) {
			GImGui = (decltype(GImGui))context;
		}

		/*
		 * Reflectible
		 */

		pmeta_get_class_name(ImGuiComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&ImGuiComponent::display)
		);
		pmeta_get_methods();
		pmeta_get_parents();
	};
}
