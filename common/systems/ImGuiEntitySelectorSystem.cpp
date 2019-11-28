#include "ImGuiEntitySelectorSystem.hpp"

#include "components/ImGuiComponent.hpp"
#include "components/SelectedComponent.hpp"

#include "helpers/TypeHelper.hpp"
#include "functions/Basic.hpp"
#include "functions/MatchString.hpp"
#include "functions/ImGuiEditor.hpp"

#include "helpers/ImGuiHelper.hpp"
#include "imgui.h"

#include "string.hpp"
#include "to_string.hpp"

static bool matches(const kengine::Entity & e, const char * str, kengine::EntityManager & em) {
	putils::string<1024> displayText("[%d]", e.id);

	if (strlen(str) != 0) {
		displayText += " Matches in ";

		bool matches = false;
		if (str[0] >= '0' && str[0] <= '9' && putils::parse<kengine::Entity::ID>(str) == e.id) {
			matches = true;
			displayText += "ID";
		}
		else {
			const auto types = kengine::TypeHelper::getSortedTypeEntities<
				kengine::functions::Has, kengine::functions::MatchString
			>(em);

			for (const auto & [_, type, has, matchFunc] : types) {
				if (!has->call(e) || !matchFunc->call(e, str))
					continue;

				if (displayText.size() + type->name.size() + 2 < decltype(displayText)::max_size) {
					if (matches) // Isn't the first time
						displayText += ", ";
					displayText += type->name;
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
	if (ImGui::Button(putils::string<64>("Remove##") + e.id)) {
		em.removeEntity(e);
		return false;
	}

	if (ImGui::TreeNode(displayText + "##" + e.id)) {
		kengine::ImGuiHelper::displayEntity(em, e);
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
		_em += ImGuiEntitySelector(_em);
	}
}