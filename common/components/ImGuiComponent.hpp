#pragma once

#include <functional>
#include "reflection/Reflectible.hpp"

struct ImGuiContext;
extern ImGuiContext * GImGui;

namespace kengine {
	struct ImGuiComponent {
		ImGuiComponent(const std::function<void()> & func = nullptr) : display([func](auto context) {
			setupImGuiContext(context);
			func();
		}) {}

		void setFunc(const std::function<void()> & func) {
			display = [func](auto context) {
				setupImGuiContext(context);
				func();
			};
		}

		static void setupImGuiContext(void * context) {
			GImGui = (decltype(GImGui))context;
		}

		std::function<void(void * context)> display;

		/*
		 * Reflectible
		 */

		pmeta_get_class_name(ImGuiComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&ImGuiComponent::display)
		);
		pmeta_get_methods(
			pmeta_reflectible_attribute(&ImGuiComponent::setupImGuiContext)
		);
		pmeta_get_parents();
	};
}
