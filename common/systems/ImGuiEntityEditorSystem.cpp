#include "ImGuiEntityEditorSystem.hpp"
#include "EntityManager.hpp"

#include "components/ImGuiComponent.hpp"
#include "components/SelectedComponent.hpp"
#include "components/NameComponent.hpp"
#include "components/TypeComponent.hpp"

#include "functions/Basic.hpp"
#include "functions/ImGuiEditor.hpp"
#include "helpers/TypeHelper.hpp"

#include "imgui.h"

auto ImGuiEntityEditor(kengine::EntityManager & em) {
	return [&](kengine::Entity & e) {
		auto & tool = e.attach<kengine::ImGuiToolComponent>();
		tool.enabled = true;
		tool.name = "Entity editor";

		e += kengine::ImGuiComponent([&] {
			if (!tool.enabled)
				return;

			for (auto &[selected, _] : em.getEntities<kengine::SelectedComponent>()) {
				bool open = true;

				ImGui::SetNextWindowSize({ 200.f, 200.f}, ImGuiCond_FirstUseEver);

				const auto beginWindow = [&] {
					if (selected.has<kengine::NameComponent>())
						return ImGui::Begin(putils::string<64>("%s##[%d]", selected.get<kengine::NameComponent>().name.c_str(), selected.id), &open, ImGuiWindowFlags_NoSavedSettings);
					else
						return ImGui::Begin(putils::string<64>("[%d] Entity editor", selected.id), nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
				};

				if (beginWindow()) {
					if (!selected.has<kengine::NameComponent>()) { // no title bar
						if (ImGui::Button("x"))
							selected.detach<kengine::SelectedComponent>();
						ImGui::SameLine();
					}
					ImGui::Text("Search");
					ImGui::SameLine();
					static char nameSearch[1024] = "";
					ImGui::PushItemWidth(-1.f);
					ImGui::InputText("##Search", nameSearch, sizeof(nameSearch));
					ImGui::PopItemWidth();

					ImGui::Separator();

					{
						ImGui::BeginChild("##child");

						if (ImGui::CollapsingHeader("Edit")) {
							const auto types = kengine::TypeHelper::getSortedTypeEntities<
								kengine::functions::Has, kengine::functions::EditImGui
							>(em);

							for (const auto & [e, type, has, edit] : types) {
								if (!strstr(type->name, nameSearch) || !has->call(selected))
									continue;
								if (ImGui::TreeNode(type->name + "##edit")) {
									edit->call(selected);
									ImGui::TreePop();
								}
							}
						}

						if (ImGui::CollapsingHeader("Add")) {
							const auto types = kengine::TypeHelper::getSortedTypeEntities<
								kengine::functions::Has, kengine::functions::Attach
							>(em);

							for (const auto & [e, type, has, add] : types) {
								if (!strstr(type->name, nameSearch) || has->call(selected))
									continue;
								if (ImGui::Button(type->name + "##add"))
									add->call(selected);
							}
						}

						if (ImGui::CollapsingHeader("Remove")) {
							const auto types = kengine::TypeHelper::getSortedTypeEntities<
								kengine::functions::Has, kengine::functions::Detach
							>(em);

							for (const auto & [e, type, has, remove] : types) {
								if (!strstr(type->name, nameSearch) || !has->call(selected))
									continue;
								if (ImGui::Button(type->name + "##remove"))
									remove->call(selected);
							}
						}

						ImGui::EndChild();
					}
				}
				ImGui::End();

				if (!open)
					selected.detach<kengine::SelectedComponent>();
			}
		});
	};
}

kengine::ImGuiEntityEditorSystem::ImGuiEntityEditorSystem(kengine::EntityManager & em)
	: System(em), _em(em)
{
	_em += ImGuiEntityEditor(_em);
}
