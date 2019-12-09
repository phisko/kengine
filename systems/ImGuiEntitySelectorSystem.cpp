#include "ImGuiEntitySelectorSystem.hpp"

#include "data/ImGuiComponent.hpp"
#include "data/SelectedComponent.hpp"

#include "helpers/TypeHelper.hpp"
#include "helpers/SortHelper.hpp"
#include "meta/Has.hpp"
#include "meta/MatchString.hpp"
#include "meta/ImGuiEditor.hpp"

#include "helpers/ImGuiHelper.hpp"
#include "imgui.h"

#include "string.hpp"
#include "to_string.hpp"

namespace kengine {
	static bool matches(const Entity & e, const char * str, EntityManager & em);
	EntityCreatorFunctor<64> ImGuiEntitySelectorSystem(EntityManager & em) {
		return [&](Entity & e) {
			auto & tool = e.attach<ImGuiToolComponent>();
			tool.enabled = true;
			tool.name = "Entity selector";

			e += ImGuiComponent([&] {
				if (!tool.enabled)
					return;

				if (ImGui::Begin("Entity selector", &tool.enabled)) {
					static char nameSearch[1024];
					ImGui::InputText("Search", nameSearch, sizeof(nameSearch));
					ImGui::SameLine();
					if (ImGui::Button("New"))
						em += [](Entity & e) { e += SelectedComponent{}; };
					ImGui::Separator();

					ImGui::BeginChild("child");
					for (auto & e : em.getEntities())
						if (matches(e, nameSearch, em)) {
							if (e.has<SelectedComponent>())
								e.detach<SelectedComponent>();
							else
								e.attach<SelectedComponent>();
						}
					ImGui::EndChild();
				}
				ImGui::End();
			});
		};
	}

	static bool matches(const Entity & e, const char * str, EntityManager & em) {
		putils::string<1024> displayText("[%d]", e.id);

		if (strlen(str) != 0) {
			displayText += " Matches in ";

			bool matches = false;
			if (str[0] >= '0' && str[0] <= '9' && putils::parse<Entity::ID>(str) == e.id) {
				matches = true;
				displayText += "ID";
			}
			else {
				const auto types = SortHelper::getNameSortedEntities<KENGINE_COMPONENT_COUNT,
					meta::Has, meta::MatchString
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
			ImGuiHelper::displayEntity(em, e);
			ImGui::TreePop();
		}

		return ret;
	}
}