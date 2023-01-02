#include "imgui_entity_selector.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// putils
#include "putils/forward_to.hpp"
#include "putils/string.hpp"
#include "putils/to_string.hpp"

// kengine data
#include "kengine/data/imgui_tool.hpp"
#include "kengine/data/name.hpp"
#include "kengine/data/selected.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine meta
#include "kengine/meta/has.hpp"
#include "kengine/meta/match_string.hpp"

// kengine helpers
#include "kengine/helpers/entity_to_string.hpp"
#include "kengine/helpers/imgui_helper.hpp"
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"
#include "kengine/helpers/sort_helper.hpp"

namespace kengine::systems {
	struct imgui_entity_selector {
		entt::registry & r;
		bool * enabled;

		imgui_entity_selector(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "Init", "systems/imgui_entity_selector");

			e.emplace<functions::execute>(putils_forward_to_this(execute));

			e.emplace<data::name>("Entity selector");
			auto & tool = e.emplace<data::imgui_tool>(true);
			enabled = &tool.enabled;
		}

		char name_search[1024];
		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!*enabled)
				return;
			kengine_log(r, verbose, "execute", "imgui_entity_selector");

			if (ImGui::Begin("Entity selector", enabled)) {
				ImGui::InputText("Search", name_search, sizeof(name_search));

				ImGui::Separator();

				ImGui::BeginChild("child");
				r.each([&](entt::entity e) {
					if (matches(e, name_search)) {
						if (r.all_of<data::selected>(e))
							r.erase<data::selected>(e);
						else
							r.emplace<data::selected>(e);
					}
				});
				ImGui::EndChild();
			}
			ImGui::End();
		}

		bool matches(entt::entity e, const char * str) noexcept {
			KENGINE_PROFILING_SCOPE;

			putils::string<1024> display_text("[%d]", e);
			const auto name = r.try_get<data::name>(e);
			if (name) {
				display_text += " ";
				display_text += name->name;
			}

			if (strlen(str) != 0) {
				display_text += " -- ";

				bool matches = false;
				if (str[0] >= '0' && str[0] <= '9' && putils::parse<entt::entity>(str) == e) {
					matches = true;
					display_text += "ID";
				}
				else {
					const auto types = sort_helper::get_name_sorted_entities<const meta::has, const meta::match_string>(r);

					for (const auto & [_, type, has, match_func] : types) {
						if (!has->call({ r, e }) || !match_func->call({ r, e }, str))
							continue;

						if (display_text.size() + type->name.size() + 2 < decltype(display_text)::max_size) {
							if (matches) // Isn't the first time
								display_text += ", ";
							display_text += type->name;
						}
						matches = true;
					}
				}

				if (!matches)
					return false;
			}

			bool ret = false;
			const auto open_tree_node = ImGui::TreeNode((display_text + "##" + int(e)).c_str());
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Select"))
					ret = true;
				if (ImGui::MenuItem("Remove")) {
					r.destroy(e);
					return false;
				}
				ImGui::EndPopup();
			}
			if (open_tree_node) {
				imgui_helper::display_entity({ r, e });
				ImGui::TreePop();
			}

			return ret;
		}
	};

	void add_imgui_entity_selector(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<imgui_entity_selector>(e);
	}
}