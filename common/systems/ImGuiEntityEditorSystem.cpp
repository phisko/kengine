#include "ImGuiEntityEditorSystem.hpp"
#include "EntityManager.hpp"

#include "components/ImGuiComponent.hpp"
#include "components/SelectedComponent.hpp"
#include "components/NameComponent.hpp"

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
						auto components = em.getComponentFunctionMaps();
						std::sort(components.begin(), components.end(), [](const auto lhs, const auto rhs) {
							return strcmp(lhs->name, rhs->name) < 0;
						});

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
