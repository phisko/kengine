#include "ImGuiHelper.hpp"
#include "EntityManager.hpp"

#include "helpers/TypeHelper.hpp"
#include "functions/Basic.hpp"
#include "functions/ImGuiEditor.hpp"
#include "imgui.h"

namespace kengine::ImGuiHelper {
	void displayEntity(EntityManager & em, const Entity & e) {
		const auto types = kengine::TypeHelper::getSortedTypeEntities<
			functions::Has, functions::DisplayImGui
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
				kengine::functions::Has, kengine::functions::EditImGui
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
				kengine::functions::Has, kengine::functions::Attach
			>(em);

			for (const auto & [_, type, has, add] : types)
				if (!has->call(e))
					if (ImGui::Button(type->name + "##add"))
						add->call(e);
		}

		if (ImGui::CollapsingHeader("Remove")) {
			const auto types = kengine::TypeHelper::getSortedTypeEntities<
				kengine::functions::Has, kengine::functions::Detach
			>(em);

			for (const auto & [_, type, has, remove] : types)
				if (has->call(e))
					if (ImGui::Button(type->name + "##remove"))
						remove->call(e);
		}
	}
}