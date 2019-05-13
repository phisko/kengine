#include "ImGuiEntityEditorSystem.hpp"
#include "EntityManager.hpp"
#include "components/ImGuiComponent.hpp"
#include "components/SelectedComponent.hpp"
#include "packets/AddImGuiTool.hpp"
#include "imgui.h"

auto ImGuiEntityEditor(kengine::EntityManager & em, const std::vector<kengine::packets::RegisterComponentEditor> & comps) {
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

					if (ImGui::CollapsingHeader("Edit"))
						for (const auto & p : comps)
							if (strstr(p.name, nameSearch) && p.has(selected))
								if (ImGui::TreeNode(p.name)) {
									p.edit(selected);
									ImGui::TreePop();
								}

					if (ImGui::CollapsingHeader("Add"))
						for (const auto & p : comps)
							if (strstr(p.name, nameSearch) && !p.has(selected))
								if (ImGui::Button(p.name))
									p.add(selected);

					if (ImGui::CollapsingHeader("Remove"))
						for (const auto & p : comps)
							if (strstr(p.name, nameSearch) && p.has(selected))
								if (ImGui::Button(putils::string<64>(p.name) + "##remove"))
									p.remove(selected);
					ImGui::End();
				}
				if (!open)
					selected.detach<kengine::SelectedComponent>();
			}
		});
	};
}

void kengine::ImGuiEntityEditorSystem::onLoad(const char * directory) noexcept {
	_em += ImGuiEntityEditor(_em, _comps);
}

void kengine::ImGuiEntityEditorSystem::handle(kengine::packets::RegisterComponentEditor p) {
	_comps.push_back(p);
	std::sort(_comps.begin(), _comps.end(), [](const packets::RegisterComponentEditor & l, const packets::RegisterComponentEditor & r) {
		return strcmp(l.name, r.name) < 0;
	});
}
