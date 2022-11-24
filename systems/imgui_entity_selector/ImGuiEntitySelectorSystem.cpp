#include "ImGuiEntitySelectorSystem.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// putils
#include "forward_to.hpp"
#include "string.hpp"
#include "to_string.hpp"

// kengine data
#include "data/ImGuiToolComponent.hpp"
#include "data/NameComponent.hpp"
#include "data/SelectedComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine meta
#include "meta/Has.hpp"
#include "meta/MatchString.hpp"

// kengine helpers
#include "helpers/entityToString.hpp"
#include "helpers/imguiHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"
#include "helpers/sortHelper.hpp"

namespace kengine {
	struct ImGuiEntitySelectorSystem {
		entt::registry & r;
		bool * enabled;

		ImGuiEntitySelectorSystem(entt::handle e) noexcept
			: r(*e.registry())
		{
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "ImGuiEntitySelectorSystem");

			e.emplace<functions::Execute>(putils_forward_to_this(execute));

			e.emplace<NameComponent>("Entity selector");
			auto & tool = e.emplace<ImGuiToolComponent>(true);
			enabled = &tool.enabled;
		}

		char nameSearch[1024];
		void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!*enabled)
				return;
			kengine_log(r, Verbose, "Execute", "ImGuiEntitySelector");

			if (ImGui::Begin("Entity selector", enabled)) {
				ImGui::InputText("Search", nameSearch, sizeof(nameSearch));

				ImGui::Separator();

				ImGui::BeginChild("child");
				r.each([&](entt::entity e) {
					if (matches(e, nameSearch)) {
						if (r.all_of<SelectedComponent>(e))
							r.erase<SelectedComponent>(e);
						else
							r.emplace<SelectedComponent>(e);
					}
				});
				ImGui::EndChild();
			}
			ImGui::End();
		}

		bool matches(entt::entity e, const char * str) noexcept {
			KENGINE_PROFILING_SCOPE;

			putils::string<1024> displayText("[%d]", e);;
			const auto name = r.try_get<NameComponent>(e);
			if (name) {
				displayText += " ";
				displayText += name->name;
			}


			if (strlen(str) != 0) {
				displayText += " -- ";

				bool matches = false;
				if (str[0] >= '0' && str[0] <= '9' && putils::parse<entt::entity>(str) == e) {
					matches = true;
					displayText += "ID";
				} else {
					const auto types = sortHelper::getNameSortedEntities<const meta::Has, const meta::MatchString>(r);

					for (const auto & [_, type, has, matchFunc]: types) {
						if (!has->call({r, e}) || !matchFunc->call({ r, e }, str))
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
			const auto openTreeNode = ImGui::TreeNode((displayText + "##" + int(e)).c_str());
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Select"))
					ret = true;
				if (ImGui::MenuItem("Remove")) {
					r.destroy(e);
					return false;
				}
				ImGui::EndPopup();
			}
			if (openTreeNode) {
				imguiHelper::displayEntity({ r, e });
				ImGui::TreePop();
			}

			return ret;
		}
	};

	void addImGuiEntitySelectorSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<ImGuiEntitySelectorSystem>(e);
	}
}