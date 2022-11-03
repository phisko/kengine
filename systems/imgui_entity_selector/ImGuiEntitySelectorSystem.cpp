#include "ImGuiEntitySelectorSystem.hpp"

// imgui
#include <imgui.h>

// putils
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
#include "helpers/logHelper.hpp"
#include "helpers/sortHelper.hpp"
#include "helpers/imguiHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::imgui_entity_selector {
	struct impl {
		static inline bool * enabled;

		static void init(Entity & e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Log, "Init", "ImGuiEntitySelector");

			e += NameComponent{ "Entity selector" };
			auto & tool = e.attach<ImGuiToolComponent>();
			tool.enabled = true;
			enabled = &tool.enabled;

			e += functions::Execute{ execute };
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!*enabled)
				return;
			kengine_log(Verbose, "Execute", "ImGuiEntitySelector");

			if (ImGui::Begin("Entity selector", enabled)) {
				static char nameSearch[1024];
				ImGui::InputText("Search", nameSearch, sizeof(nameSearch));

				ImGui::Separator();

				ImGui::BeginChild("child");
				for (auto e : entities)
					if (matches(e, nameSearch)) {
						if (e.has<SelectedComponent>())
							e.detach<SelectedComponent>();
						else
							e.attach<SelectedComponent>();
					}
				ImGui::EndChild();
			}
			ImGui::End();
		}

		static bool matches(const Entity & e, const char * str) noexcept {
			KENGINE_PROFILING_SCOPE;

			putils::string<1024> displayText("[%d]", e.id);;
			const auto name = e.tryGet<NameComponent>();
			if (name) {
				displayText += " ";
				displayText += name->name;
			}


			if (strlen(str) != 0) {
				displayText += " -- ";

				bool matches = false;
				if (str[0] >= '0' && str[0] <= '9' && putils::parse<EntityID>(str) == e.id) {
					matches = true;
					displayText += "ID";
				}
				else {
					const auto types = sortHelper::getNameSortedEntities<KENGINE_COMPONENT_COUNT,
						meta::Has, meta::MatchString
					>();

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
			const auto openTreeNode = ImGui::TreeNode(displayText + "##" + e.id);
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Select"))
					ret = true;
				if (ImGui::MenuItem("Remove")) {
					entities -= e;
					return false;
				}
				ImGui::EndPopup();
			}
			if (openTreeNode) {
				imguiHelper::displayEntity(e);
				ImGui::TreePop();
			}

			return ret;
		}
	};
}

namespace kengine {
	EntityCreator * ImGuiEntitySelectorSystem() noexcept {
		KENGINE_PROFILING_SCOPE;
		return [](Entity & e) noexcept {
			imgui_entity_selector::impl::init(e);
		};
	}
}