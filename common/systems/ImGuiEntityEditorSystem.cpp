#include "ImGuiEntityEditorSystem.hpp"
#include "EntityManager.hpp"

#include "components/ImGuiComponent.hpp"
#include "components/SelectedComponent.hpp"

#include "functions/Basic.hpp"
#include "functions/ImGuiEditor.hpp"

#include "packets/AddImGuiTool.hpp"

#include "imgui.h"

auto ImGuiEntityEditor(kengine::EntityManager & em) {
	return [&](kengine::Entity & e) {
		static bool display = true;
		em.send(kengine::packets::AddImGuiTool{ "Entity editor", display });

		e += kengine::ImGuiComponent([&] {
			if (!display)
				return;

			for (auto &[selected, _] : em.getEntities<kengine::SelectedComponent>()) {
				bool open = true;
				if (ImGui::Begin(putils::string<64>("[%d] Entity editor", selected.id), &open)) {
					static char nameSearch[1024] = "";
					ImGui::InputText("Component", nameSearch, sizeof(nameSearch));

					const auto components = em.getComponentFunctionMaps();
					if (ImGui::CollapsingHeader("Edit"))
						for (const auto comp : components) {
							const auto has = comp->getFunction<kengine::functions::Has>();
							const auto edit = comp->getFunction<kengine::functions::EditImGui>();
							if (has != nullptr && edit != nullptr && strstr(comp->name, nameSearch) && has(selected))
								if (ImGui::TreeNode(comp->name)) {
									edit(selected);
									ImGui::TreePop();
								}
						}

					if (ImGui::CollapsingHeader("Add"))
						for (const auto comp : components) {
							const auto has = comp->getFunction<kengine::functions::Has>();
							const auto add = comp->getFunction<kengine::functions::Attach>();
							if (has != nullptr && add != nullptr && strstr(comp->name, nameSearch) && !has(selected))
								if (ImGui::Button(comp->name))
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
				}
				ImGui::End();
				if (!open)
					selected.detach<kengine::SelectedComponent>();
			}
		});
	};
}

void kengine::ImGuiEntityEditorSystem::onLoad(const char * directory) noexcept {
	_em += ImGuiEntityEditor(_em);
}
