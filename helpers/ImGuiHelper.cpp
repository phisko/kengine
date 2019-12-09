#include "ImGuiHelper.hpp"
#include "EntityManager.hpp"

#include "helpers/TypeHelper.hpp"
#include "meta/Has.hpp"
#include "meta/AttachTo.hpp"
#include "meta/DetachFrom.hpp"
#include "meta/ImGuiEditor.hpp"
#include "imgui.h"

namespace kengine::ImGuiHelper {
	void displayEntity(EntityManager & em, const Entity & e) {
		const auto types = kengine::TypeHelper::getSortedTypeEntities<
			meta::Has, meta::DisplayImGui
		>(em);

		for (const auto & [_, type, has, display] : types)
			if (has->call(e))
				if (ImGui::TreeNode(type->name)) {
					display->call(e);
					ImGui::TreePop();
				}
	}

	void editEntity(EntityManager & em, Entity & e) {
		if (ImGui::CollapsingHeader("Edit")) {
			const auto types = kengine::TypeHelper::getSortedTypeEntities<
				meta::Has, meta::EditImGui
			>(em);

			for (const auto & [_, type, has, edit] : types)
				if (has->call(e))
					if (ImGui::TreeNode(type->name + "##edit")) {
						edit->call(e);
						ImGui::TreePop();
					}
		}

		if (ImGui::CollapsingHeader("Add")) {
			const auto types = kengine::TypeHelper::getSortedTypeEntities<
				meta::Has, meta::AttachTo
			>(em);

			for (const auto & [_, type, has, add] : types)
				if (!has->call(e))
					if (ImGui::Button(type->name + "##add"))
						add->call(e);
		}

		if (ImGui::CollapsingHeader("Remove")) {
			const auto types = kengine::TypeHelper::getSortedTypeEntities<
				meta::Has, meta::DetachFrom
			>(em);

			for (const auto & [_, type, has, remove] : types)
				if (has->call(e))
					if (ImGui::Button(type->name + "##remove"))
						remove->call(e);
		}
	}
}