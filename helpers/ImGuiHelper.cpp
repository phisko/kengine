#include "ImGuiHelper.hpp"
#include "EntityManager.hpp"

#include "helpers/TypeHelper.hpp"
#include "helpers/SortHelper.hpp"
#include "meta/Has.hpp"
#include "meta/AttachTo.hpp"
#include "meta/DetachFrom.hpp"
#include "meta/DisplayImGui.hpp"
#include "meta/EditImGui.hpp"
#include "imgui.h"

namespace kengine::ImGuiHelper {
	void displayEntity(EntityManager & em, const Entity & e) {
		const auto types = SortHelper::getNameSortedEntities<KENGINE_COMPONENT_COUNT,
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
		if (ImGui::CollapsingHeader("Edit")) {
			const auto types = SortHelper::getNameSortedEntities<KENGINE_COMPONENT_COUNT,
				meta::Has, meta::EditImGui
			>(em);

			for (const auto & [_, name, has, edit] : types)
				if (has->call(e))
					if (ImGui::TreeNode(name->name + "##edit")) {
						edit->call(e);
						ImGui::TreePop();
					}
		}

		if (ImGui::CollapsingHeader("Add")) {
			const auto types = SortHelper::getNameSortedEntities<KENGINE_COMPONENT_COUNT,
				meta::Has, meta::AttachTo
			>(em);

			for (const auto & [_, name, has, add] : types)
				if (!has->call(e))
					if (ImGui::Button(name->name + "##add"))
						add->call(e);
		}

		if (ImGui::CollapsingHeader("Remove")) {
			const auto types = SortHelper::getNameSortedEntities<KENGINE_COMPONENT_COUNT,
				meta::Has, meta::DetachFrom
			>(em);

			for (const auto & [_, name, has, remove] : types)
				if (has->call(e))
					if (ImGui::Button(name->name + "##remove"))
						remove->call(e);
		}
	}
}