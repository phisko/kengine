#include "ImGuiEntitySelectorSystem.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

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
#include "helpers/entityToString.hpp"
#include "helpers/imguiHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"
#include "helpers/sortHelper.hpp"

namespace kengine {
	struct ImGuiEntitySelectorSystem {
		static void init(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "ImGuiEntitySelectorSystem");

			_r = &r;

			const auto e = r.create();
			r.emplace<functions::Execute>(e, execute);

			r.emplace<NameComponent>(e, "Entity selector");
			auto & tool = r.emplace<ImGuiToolComponent>(e, true);
			_enabled = &tool.enabled;
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!*_enabled)
				return;
			kengine_log(*_r, Verbose, "Execute", "ImGuiEntitySelector");

			if (ImGui::Begin("Entity selector", _enabled)) {
				static char nameSearch[1024];
				ImGui::InputText("Search", nameSearch, sizeof(nameSearch));

				ImGui::Separator();

				ImGui::BeginChild("child");
				_r->each([&](entt::entity e) {
					if (matches(e, nameSearch)) {
						if (_r->all_of<SelectedComponent>(e))
							_r->erase<SelectedComponent>(e);
						else
							_r->emplace<SelectedComponent>(e);
					}
				});
				ImGui::EndChild();
			}
			ImGui::End();
		}

		static bool matches(entt::entity e, const char * str) noexcept {
			KENGINE_PROFILING_SCOPE;

			putils::string<1024> displayText("[%d]", e);;
			const auto name = _r->try_get<NameComponent>(e);
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
					const auto types = sortHelper::getNameSortedEntities<const meta::Has, const meta::MatchString>(*_r);

					for (const auto & [_, type, has, matchFunc]: types) {
						if (!has->call({*_r, e}) || !matchFunc->call({ *_r, e }, str))
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
					_r->destroy(e);
					return false;
				}
				ImGui::EndPopup();
			}
			if (openTreeNode) {
				imguiHelper::displayEntity({ *_r, e });
				ImGui::TreePop();
			}

			return ret;
		}

		static inline bool * _enabled;
		static inline entt::registry * _r;
	};

	void ImGuiEntitySelectorSystem(entt::registry & r) noexcept {
		ImGuiEntitySelectorSystem::init(r);
	}
}