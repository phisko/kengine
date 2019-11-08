#pragma once

#ifndef KENGINE_IMGUI_FUNCTION_SIZE
# define KENGINE_IMGUI_FUNCTION_SIZE 64
#endif

#ifndef KENGINE_IMGUI_TOOL_NAME_MAX_LENGTH
# define KENGINE_IMGUI_TOOL_NAME_MAX_LENGTH 64
#endif

#include "function.hpp"
#include "reflection.hpp"
#include "not_serializable.hpp"

struct ImGuiContext;
extern ImGuiContext * GImGui;

namespace kengine {
	struct ImGuiComponent : kengine::not_serializable {
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

		pmeta_get_class_name(ImGuiComponent);
		pmeta_get_attributes(
			pmeta_reflectible_attribute(&ImGuiComponent::display)
		);
		pmeta_get_methods(
			pmeta_reflectible_attribute(&ImGuiComponent::setupImGuiContext)
		);
		pmeta_get_parents();
	};

	struct ImGuiToolComponent : kengine::not_serializable {
		static constexpr char stringName[] = "ImGuiToolComponentString";
		using string = putils::string<KENGINE_IMGUI_TOOL_NAME_MAX_LENGTH, stringName>;
		string name;
		bool enabled;
	};
}
