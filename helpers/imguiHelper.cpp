#include "ImGuiHelper.hpp"
#include "kengine.hpp"

#include "helpers/typeHelper.hpp"
#include "helpers/sortHelper.hpp"

#include "data/ImGuiScaleComponent.hpp"

#include "meta/Has.hpp"
#include "meta/AttachTo.hpp"
#include "meta/DetachFrom.hpp"
#include "meta/DisplayImGui.hpp"
#include "meta/EditImGui.hpp"

#include "imgui.h"

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

	float getScale() noexcept {
		float scale = 1.f;
		for (const auto & [e, comp] : entities.with<ImGuiScaleComponent>())
			scale *= comp.scale;
		return scale;
	}
}