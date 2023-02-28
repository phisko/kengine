#include "edit_entity.hpp"

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
#include "kengine/meta/functions/emplace_or_replace.hpp"
#include "kengine/meta/functions/remove.hpp"
#include "kengine/meta/helpers/get_type_entity.hpp"

// kengine meta/imgui
#include "kengine/meta/imgui/functions/edit.hpp"

// kengine imgui
#include "kengine/imgui/helpers/set_context.hpp"

// kengine instance
#include "kengine/instance/data/instance.hpp"

namespace kengine::meta::imgui {
	static constexpr auto log_category = "meta_imgui";

	void edit_entity(entt::handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), very_verbose, log_category, "Editing [%u]", e.entity());

		const auto & r = *e.registry();

		if (!kengine::imgui::set_context(r))
			return;

		if (ImGui::BeginPopupContextWindow()) {
			const auto types = core::sort::get_name_sorted_entities<const has, const emplace_or_replace>(r);

			for (const auto & [_, name, has, emplace_or_replace] : types)
				if (!has->call(e))
					if (ImGui::MenuItem(name->name.c_str()))
						emplace_or_replace->call(e, nullptr);

			ImGui::EndPopup();
		}

		const auto types = core::sort::get_name_sorted_entities<const has, const edit>(r);

		for (const auto & [type_entity, name, has, edit] : types) {
			if (!has->call(e))
				continue;
			const auto tree_node_open = ImGui::TreeNode((name->name + "##edit").c_str());

			if (const auto remove = r.try_get<meta::remove>(type_entity)) {
				if (ImGui::BeginPopupContextItem()) {
					if (ImGui::MenuItem("Remove"))
						remove->call(e);
					ImGui::EndPopup();
				}
			}
			if (tree_node_open) {
				edit->call(e);
				ImGui::TreePop();
			}
		}
	}

	void edit_entity_and_model(entt::handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), very_verbose, log_category, "Editing [%u] and its model", e.entity());

		if (!kengine::imgui::set_context(*e.registry()))
			return;

		const auto instance = e.try_get<instance::instance>();
		if (!instance || instance->model == entt::null) {
			kengine_log(*e.registry(), very_verbose, log_category, "No model found");
			edit_entity(e);
			return;
		}

		kengine_logf(*e.registry(), very_verbose, log_category, "Found model [%u]", instance->model);
		if (ImGui::BeginTabBar("##tabs")) {
			ImGui::PushItemWidth(ImGui::GetWindowWidth() / 2.f);

			if (ImGui::BeginTabItem("object")) {
				edit_entity(e);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Model")) {
				edit_entity({ *e.registry(), instance->model });
				ImGui::EndTabItem();
			}

			ImGui::PopItemWidth();

			ImGui::EndTabBar();
		}
	}
}