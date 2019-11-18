#include "ImGuiEntityEditorSystem.hpp"
#include "EntityManager.hpp"

#include "components/ImGuiComponent.hpp"
#include "components/SelectedComponent.hpp"

#include "functions/Basic.hpp"
#include "functions/ImGuiEditor.hpp"

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
				ImGui::SetNextWindowSize({ 200.f, 200.f}, ImGuiCond_FirstUseEver);

				if (ImGui::Begin(putils::string<64>("[%d] Entity editor", selected.id), nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar)) {

					if (ImGui::Button("x"))
						selected.detach<kengine::SelectedComponent>();
					ImGui::SameLine();
					ImGui::Text("Search");
					ImGui::SameLine();
					static char nameSearch[1024] = "";
					ImGui::PushItemWidth(-1.f);
					ImGui::InputText("##Search", nameSearch, sizeof(nameSearch));
					ImGui::PopItemWidth();

					ImGui::Separator();

					{
						ImGui::BeginChild("##child");
						const auto components = em.getComponentFunctionMaps();
						if (ImGui::CollapsingHeader("Edit"))
							for (const auto comp : components) {
								const auto has = comp->getFunction<kengine::functions::Has>();
								const auto edit = comp->getFunction<kengine::functions::EditImGui>();
								if (has != nullptr && edit != nullptr && strstr(comp->name, nameSearch) && has(selected))
									if (ImGui::TreeNode(putils::string<64>(comp->name) + "##edit")) {
										edit(selected);
										ImGui::TreePop();
									}
							}

						if (ImGui::CollapsingHeader("Add"))
							for (const auto comp : components) {
								const auto has = comp->getFunction<kengine::functions::Has>();
								const auto add = comp->getFunction<kengine::functions::Attach>();
								if (has != nullptr && add != nullptr && strstr(comp->name, nameSearch) && !has(selected))
									if (ImGui::Button(putils::string<64>(comp->name) + "##add"))
										add(selected);
							}

						if (ImGui::CollapsingHeader("Remove"))
							for (const auto comp : components) {
								const auto has = comp->getFunction<kengine::functions::Has>();
								const auto remove = comp->getFunction<kengine::functions::Detach>();
								if (has != nullptr && remove != nullptr && strstr(comp->name, nameSearch) && has(selected))
									if (ImGui::Button(putils::string<64>(comp->name) + "##remove"))
										remove(selected);
							}

						ImGui::EndChild();
					}
				}
				ImGui::End();
			}
		});
	};
}

kengine::ImGuiEntityEditorSystem::ImGuiEntityEditorSystem(kengine::EntityManager & em)
	: System(em), _em(em)
{
	_em += ImGuiEntityEditor(_em);
}
