#include "ImGuiHelper.hpp"
#include "EntityManager.hpp"

#include "helpers/typeHelper.hpp"
#include "helpers/sortHelper.hpp"
#include "meta/Has.hpp"
#include "meta/AttachTo.hpp"
#include "meta/DetachFrom.hpp"
#include "meta/DisplayImGui.hpp"
#include "meta/EditImGui.hpp"
#include "imgui.h"

namespace kengine::imguiHelper {
	void displayEntity(EntityManager & em, const Entity & e) {
		const auto types = sortHelper::getNameSortedEntities<KENGINE_COMPONENT_COUNT,
			meta::Has, meta::DisplayImGui
		>(em);

		for (const auto & [_, name, has, display] : types)
			if (has->call(e))
				if (ImGui::TreeNode(name->name)) {
					display->call(e);
					ImGui::TreePop();
				}
	}

	void editEntity(EntityManager & em, Entity & e) {
		if (ImGui::BeginPopupContextWindow()) {
			const auto types = sortHelper::getNameSortedEntities<KENGINE_COMPONENT_COUNT,
				meta::Has, meta::AttachTo
			>(em);

			for (const auto & [_, name, has, add] : types)
				if (!has->call(e))
					if (ImGui::MenuItem(name->name))
						add->call(e);

			ImGui::EndPopup();
		}

		const auto types = sortHelper::getNameSortedEntities<KENGINE_COMPONENT_COUNT,
			meta::Has, meta::EditImGui
		>(em);

		for (const auto & [_, name, has, edit] : types) {
			if (!has->call(e))
				continue;
			const auto treeNodeOpen = ImGui::TreeNode(name->name + "##edit");
			if (_.has<meta::DetachFrom>()) {
				if (ImGui::BeginPopupContextItem()) {
					if (ImGui::MenuItem("Remove"))
						_.get<meta::DetachFrom>()(e);
					ImGui::EndPopup();
				}
			}
			if (treeNodeOpen) {
				edit->call(e);
				ImGui::TreePop();
			}
		}
	}
}