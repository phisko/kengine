#include "imguiHelper.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// kengine data
#include "data/ImGuiScaleComponent.hpp"
#include "data/InstanceComponent.hpp"

// kengine meta
#include "meta/Has.hpp"
#include "meta/AttachTo.hpp"
#include "meta/DetachFrom.hpp"
#include "meta/DisplayImGui.hpp"
#include "meta/EditImGui.hpp"

// kengine helpers
#include "helpers/typeHelper.hpp"
#include "helpers/sortHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::imguiHelper {
	void displayEntity(entt::const_handle e) noexcept {
		KENGINE_PROFILING_SCOPE;

		const auto types = sortHelper::getNameSortedEntities<const meta::Has, const meta::DisplayImGui>(*e.registry());

		for (const auto & [_, name, has, display] : types)
			if (has->call(e))
				if (ImGui::TreeNode(name->name.c_str())) {
					display->call(e);
					ImGui::TreePop();
				}
	}

	void displayEntityAndModel(entt::const_handle e) noexcept {
		KENGINE_PROFILING_SCOPE;

		const auto instance = e.try_get<InstanceComponent>();
		if (!instance || instance->model == entt::null) {
			displayEntity(e);
			return;
		}

		if (ImGui::BeginTabBar("##tabs")) {
			if (ImGui::BeginTabItem("Object")) {
				displayEntity(e);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Model")) {
				displayEntity({ *e.registry(), instance->model });
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}

	void editEntity(entt::handle e) noexcept {
		KENGINE_PROFILING_SCOPE;

		const auto & r = *e.registry();

		if (ImGui::BeginPopupContextWindow()) {
			const auto types = sortHelper::getNameSortedEntities<const meta::Has, const meta::AttachTo>(r);

			for (const auto & [_, name, has, add] : types)
				if (!has->call(e))
					if (ImGui::MenuItem(name->name.c_str()))
						add->call(e);

			ImGui::EndPopup();
		}

		const auto types = sortHelper::getNameSortedEntities<const meta::Has, const meta::EditImGui>(r);

		for (const auto & [typeEntity, name, has, edit] : types) {
			if (!has->call(e))
				continue;
			const auto treeNodeOpen = ImGui::TreeNode((name->name + "##edit").c_str());

			const auto detachFrom = r.try_get<meta::DetachFrom>(typeEntity);
			if (detachFrom) {
				if (ImGui::BeginPopupContextItem()) {
					if (ImGui::MenuItem("Remove"))
						detachFrom->call(e);
					ImGui::EndPopup();
				}
			}
			if (treeNodeOpen) {
				edit->call(e);
				ImGui::TreePop();
			}
		}
	}

	void editEntityAndModel(entt::handle e) noexcept {
		KENGINE_PROFILING_SCOPE;

		const auto instance = e.try_get<InstanceComponent>();
		if (!instance || instance->model == entt::null) {
			editEntity(e);
			return;
		}

		if (ImGui::BeginTabBar("##tabs")) {
			ImGui::PushItemWidth(ImGui::GetWindowWidth() / 2.f);

			if (ImGui::BeginTabItem("Object")) {
				editEntity(e);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Model")) {
				editEntity({ *e.registry(), instance->model });
				ImGui::EndTabItem();
			}

			ImGui::PopItemWidth();

			ImGui::EndTabBar();
		}
	}

	float getScale(const entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		float scale = 1.f;
		for (const auto & [e, comp] : r.view<ImGuiScaleComponent>().each())
			scale *= comp.scale;
		return scale;
	}
}