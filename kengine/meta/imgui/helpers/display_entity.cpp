#include "display_entity.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// kengine core
#include "kengine/core/sort/helpers/get_name_sorted_entities.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine meta
#include "kengine/meta/functions/has.hpp"
#include "kengine/meta/helpers/get_type_entity.hpp"

// kengine meta/imgui
#include "kengine/meta/imgui/functions/display.hpp"

// kengine imgui
#include "kengine/imgui/helpers/set_context.hpp"

// kengine instance
#include "kengine/instance/data/instance.hpp"

namespace kengine::meta::imgui {
	static constexpr auto log_category = "meta_imgui";

	void display_entity(entt::const_handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), very_verbose, log_category, "Displaying {}", e);

		if (!kengine::imgui::set_context(*e.registry()))
			return;

		const auto types = core::sort::get_name_sorted_entities<const has, const display>(*e.registry());

		for (const auto & [_, name, has, display] : types)
			if (has->call(e))
				if (ImGui::TreeNode(name->name.c_str())) {
					display->call(e);
					ImGui::TreePop();
				}
	}

	void display_entity_and_model(entt::const_handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), very_verbose, log_category, "Displaying {} and its model", e);

		if (!kengine::imgui::set_context(*e.registry()))
			return;

		const auto instance = e.try_get<instance::instance>();
		if (!instance || instance->model == entt::null) {
			kengine_log(*e.registry(), very_verbose, log_category, "No model found");
			display_entity(e);
			return;
		}

		kengine_logf(*e.registry(), very_verbose, log_category, "Found model {}", instance->model);
		if (ImGui::BeginTabBar("##tabs")) {
			if (ImGui::BeginTabItem("object")) {
				display_entity(e);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Model")) {
				display_entity({ *e.registry(), instance->model });
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}
}