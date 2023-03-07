#include "edit_entity.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// kengine
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/core/sort/helpers/get_name_sorted_entities.hpp"
#include "kengine/imgui/helpers/set_context.hpp"
#include "kengine/model/data/instance.hpp"
#include "kengine/meta/functions/emplace_or_replace.hpp"
#include "kengine/meta/functions/has.hpp"
#include "kengine/meta/functions/remove.hpp"
#include "kengine/meta/helpers/get_type_entity.hpp"
#include "kengine/meta/imgui/functions/edit.hpp"

namespace kengine::meta::imgui {
	static constexpr auto log_category = "meta_imgui";

	bool edit_entity(entt::handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), very_verbose, log_category, "Editing {}", e);

		const auto & r = *e.registry();

		if (!kengine::imgui::set_context(r))
			return false;

		bool ret = false;
		if (ImGui::BeginPopupContextWindow()) {
			const auto types = core::sort::get_name_sorted_entities<const has, const emplace_or_replace>(r);

			for (const auto & [_, name, has, emplace_or_replace] : types)
				if (!has->call(e))
					if (ImGui::MenuItem(name->name.c_str())) {
						ret = true;
						emplace_or_replace->call(e, nullptr);
					}

			ImGui::EndPopup();
		}

		const auto types = core::sort::get_name_sorted_entities<const has, const edit>(r);

		for (const auto & [type_entity, name, has, edit] : types) {
			if (!has->call(e))
				continue;
			const auto tree_node_open = ImGui::TreeNode((name->name + "##edit").c_str());

			if (const auto remove = r.try_get<meta::remove>(type_entity)) {
				if (ImGui::BeginPopupContextItem()) {
					if (ImGui::MenuItem("Remove")) {
						ret = true;
						remove->call(e);
					}
					ImGui::EndPopup();
				}
			}
			if (tree_node_open) {
				ret |= edit->call(e);
				ImGui::TreePop();
			}
		}

		return ret;
	}

	bool edit_entity_and_model(entt::handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), very_verbose, log_category, "Editing {} and its model", e);

		if (!kengine::imgui::set_context(*e.registry()))
			return false;

		const auto instance = e.try_get<model::instance>();
		if (!instance || instance->model == entt::null) {
			kengine_log(*e.registry(), very_verbose, log_category, "No model found");
			return edit_entity(e);
		}

		kengine_logf(*e.registry(), very_verbose, log_category, "Found model {}", instance->model);

		bool ret = false;
		if (ImGui::BeginTabBar("##tabs")) {
			ImGui::PushItemWidth(ImGui::GetWindowWidth() / 2.f);

			if (ImGui::BeginTabItem("object")) {
				ret |= edit_entity(e);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Model")) {
				ret |= edit_entity({ *e.registry(), instance->model });
				ImGui::EndTabItem();
			}

			ImGui::PopItemWidth();

			ImGui::EndTabBar();
		}

		return ret;
	}
}