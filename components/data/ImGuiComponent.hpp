#pragma once

#ifndef KENGINE_IMGUI_FUNCTION_SIZE
# define KENGINE_IMGUI_FUNCTION_SIZE 64
#endif

#include "function.hpp"
#include "reflection.hpp"

struct ImGuiContext;
extern ImGuiContext * GImGui;

namespace kengine {
	struct ImGuiComponent {
		using function = putils::function<void(void * context), KENGINE_IMGUI_FUNCTION_SIZE>;

		ImGuiComponent() = default;

		template<typename Func>
		ImGuiComponent(const Func & func) : display([this, func](auto context) {
			setupImGuiContext(context);
			func();
		}) {}

		template<typename Func>
		void setFunc(Func && func) {
			display = [func](auto context) {
				setupImGuiContext(context);
				func();
			};
		}

		void setupImGuiContext(void * context) {
			GImGui = (decltype(GImGui))context;
		}

		function display = nullptr;

		/*
		 * Reflectible
		 */

		putils_reflection_class_name(ImGuiComponent);
		putils_reflection_attributes(
			putils_reflection_attribute(&ImGuiComponent::display)
		);
		putils_reflection_methods(
			putils_reflection_attribute(&ImGuiComponent::setupImGuiContext)
		);
	};
}
