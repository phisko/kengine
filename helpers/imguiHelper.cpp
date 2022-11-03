#include "imguiHelper.hpp"
#include "kengine.hpp"

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

namespace kengine::imguiHelper {
	void displayEntity(const Entity & e) noexcept {
		const auto types = sortHelper::getNameSortedEntities<KENGINE_COMPONENT_COUNT,
			meta::Has, meta::DisplayImGui
		>();

		for (const auto & [_, name, has, display] : types)
			if (has->call(e))
				if (ImGui::TreeNode(name->name)) {
					display->call(e);
					ImGui::TreePop();
				}
	}

	void displayEntityAndModel(const Entity & e) noexcept {
		const auto instance = e.tryGet<InstanceComponent>();
		if (!instance || instance->model == INVALID_ID) {
			displayEntity(e);
			return;
		}

		if (ImGui::BeginTabBar("##tabs")) {
			if (ImGui::BeginTabItem("Object")) {
				displayEntity(e);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Model")) {
				displayEntity(entities[instance->model]);
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}

	void editEntity(Entity & e) noexcept {
		if (ImGui::BeginPopupContextWindow()) {
			const auto types = sortHelper::getNameSortedEntities<KENGINE_COMPONENT_COUNT,
				meta::Has, meta::AttachTo
			>();

			for (const auto & [_, name, has, add] : types)
				if (!has->call(e))
					if (ImGui::MenuItem(name->name))
						add->call(e);

			ImGui::EndPopup();
		}

		const auto types = sortHelper::getNameSortedEntities<KENGINE_COMPONENT_COUNT,
			meta::Has, meta::EditImGui
		>();

		for (const auto & [_, name, has, edit] : types) {
			if (!has->call(e))
				continue;
			const auto treeNodeOpen = ImGui::TreeNode(name->name + "##edit");

			const auto detachFrom = _.tryGet<meta::DetachFrom>();
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

	void editEntityAndModel(Entity & e) noexcept {
		const auto instance = e.tryGet<InstanceComponent>();
		if (!instance || instance->model == INVALID_ID) {
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
				auto model = entities[instance->model];
				editEntity(model);
				ImGui::EndTabItem();
			}

			ImGui::PopItemWidth();

			ImGui::EndTabBar();
		}
	}

	float getScale() noexcept {
		float scale = 1.f;
		for (const auto & [e, comp] : entities.with<ImGuiScaleComponent>())
			scale *= comp.scale;
		return scale;
	}
}