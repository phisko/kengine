#include "ImGuiEntitySelectorSystem.hpp"

#include "components/ImGuiComponent.hpp"
#include "components/SelectedComponent.hpp"

#include "functions/Basic.hpp"
#include "functions/MatchString.hpp"
#include "functions/ImGuiEditor.hpp"

#include "imgui.h"
#include "string.hpp"
#include "to_string.hpp"

static bool matches(const kengine::Entity & e, const char * str, kengine::EntityManager & em) {
	putils::string<1024> displayText("[%d]", e.id);

	const auto components = em.getComponentFunctionMaps();

	if (strlen(str) != 0) {
		displayText += " Matches in ";

		bool matches = false;
		if (str[0] >= '0' && str[0] <= '9' && putils::parse<kengine::Entity::ID>(str) == e.id) {
			matches = true;
			displayText += "ID";
		}
		else {
			for (const auto comp : components) {
				const auto has = comp->getFunction<kengine::functions::Has>();
				const auto matchFunc = comp->getFunction<kengine::functions::MatchString>();
				if (has != nullptr && matchFunc != nullptr && has(e) && matchFunc(e, str)) {
					if (displayText.size() + strlen(comp->name) + 2 < decltype(displayText)::max_size) {
						if (matches) // Isn't the first time
							displayText += ", ";
						displayText += comp->name;
					}
					matches = true;
				}
			}
		}

		if (!matches)
			return false;
	}

	bool ret = false;
	if (ImGui::Button(putils::string<64>("Select##") + e.id))
		ret = true;
	ImGui::SameLine();
	if (ImGui::Button(putils::string<64>("Remove##") + e.id)) {
		em.removeEntity(e);
		return false;
	}
	if (ImGui::TreeNode(displayText + "##" + e.id)) {
		for (const auto & comp : components) {
			const auto has = comp->getFunction<kengine::functions::Has>();
			const auto display = comp->getFunction<kengine::functions::DisplayImGui>();
			if (has != nullptr && display != nullptr && has(e))
				if (ImGui::TreeNode(comp->name)) {
					display(e);
					ImGui::TreePop();
				}
		}
		ImGui::TreePop();
	}

	return ret;
}

static auto ImGuiEntitySelector(kengine::EntityManager & em) {
	return [&](kengine::Entity & e) {
		auto & tool = e.attach<kengine::ImGuiToolComponent>();
		tool.enabled = true;
		tool.name = "Entity selector";

		e += kengine::ImGuiComponent([&] {
			if (!tool.enabled)
				return;

			if (ImGui::Begin("Entity selector", &tool.enabled)) {
				static char nameSearch[1024];
				ImGui::InputText("Search", nameSearch, sizeof(nameSearch));
				ImGui::SameLine();
				if (ImGui::Button("New"))
					em += [](kengine::Entity & e) { e += kengine::SelectedComponent{}; };
				ImGui::Separator();

				ImGui::BeginChild("child");
				for (auto & e : em.getEntities())
					if (matches(e, nameSearch, em)) {
						if (e.has<kengine::SelectedComponent>())
							e.detach<kengine::SelectedComponent>();
						else
							e.attach<kengine::SelectedComponent>();
					}
				ImGui::EndChild();
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
		_em += ImGuiEntitySelector(_em);
	}
}