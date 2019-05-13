#include "ImGuiEntitySelectorSystem.hpp"

#include "components/ImGuiComponent.hpp"
#include "components/SelectedComponent.hpp"

#include "packets/AddImGuiTool.hpp"

#include "imgui.h"
#include "string.hpp"
#include "to_string.hpp"

static bool matches(const kengine::Entity & e, const char * str, const std::vector<kengine::packets::RegisterComponentEditor> & comps) {
	putils::string<1024> displayText("[%d]", e.id);

	if (strlen(str) != 0) {
		displayText += " Matches in ";

		bool matches = false;
		if (str[0] >= '0' && str[0] <= '9' && putils::parse<kengine::Entity::ID>(str) == e.id) {
			matches = true;
			displayText += "ID";
		}
		else {
			for (const auto & comp : comps)
				if (comp.has(e) && comp.matches(e, str)) {
					if (displayText.size() + strlen(comp.name) + 2 < decltype(displayText)::max_size) {
						if (matches) // Isn't the first time
							displayText += ", ";
						displayText += comp.name;
					}
					matches = true;
				}
		}

		if (!matches)
			return false;
	}

	bool ret = false;
	if (ImGui::Button(putils::string<64>("Select##") + e.id))
		ret = true;
	ImGui::SameLine();
	if (ImGui::TreeNode(displayText + "##" + e.id)) {
		for (const auto & comp : comps) {
			if (comp.has(e))
				if (ImGui::TreeNode(comp.name)) {
					comp.display(e);
					ImGui::TreePop();
				}
		}
		ImGui::TreePop();
	}

	return ret;
}

static auto ImGuiEntitySelector(kengine::EntityManager & em, const std::vector<kengine::packets::RegisterComponentEditor> & comps) {
	return [&](kengine::Entity & e) {
		static bool display = true;
		em.send(kengine::packets::AddImGuiTool{ "Entity selector", display });
		e += kengine::ImGuiComponent([&] {
			if (!display)
				return;

			if (ImGui::Begin("Entity selector", &display)) {
				static char nameSearch[1024];
				ImGui::InputText("Search", nameSearch, sizeof(nameSearch));
				ImGui::Separator();

				for (auto & e : em.getEntities())
					if (matches(e, nameSearch, comps)) {
						if (e.has<kengine::SelectedComponent>())
							e.detach<kengine::SelectedComponent>();
						else
							e.attach<kengine::SelectedComponent>();
					}
			}
			ImGui::End();
		});
	};
}

namespace kengine {
	ImGuiEntitySelectorSystem::ImGuiEntitySelectorSystem(kengine::EntityManager & em)
		: System(em), _em(em)
	{
		onLoad("");
	}

	void ImGuiEntitySelectorSystem::onLoad(const char * directory) noexcept {
		_em += ImGuiEntitySelector(_em, _comps);
	}

	void ImGuiEntitySelectorSystem::handle(packets::RegisterComponentEditor p) {
		_comps.push_back(p);
	}
}