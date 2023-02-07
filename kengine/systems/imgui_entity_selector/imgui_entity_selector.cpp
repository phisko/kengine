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

		struct search_result {
			entt::entity e;
			putils::string<1024> display_text;
		};
		std::vector<search_result> search_results;

		imgui_entity_selector(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "imgui_entity_selector", "Initializing");

			e.emplace<functions::execute>(putils_forward_to_this(execute));

			e.emplace<data::name>("Entities/Selector");
			auto & tool = e.emplace<data::imgui_tool>();
			enabled = &tool.enabled;
		}

		char name_search[1024] = "";
		bool search_out_of_date = true;
		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "imgui_entity_selector", "Executing");

			if (!*enabled) {
				kengine_log(r, very_verbose, "imgui_entity_selector", "Disabled");
				return;
			}

			if (ImGui::Begin("Entity selector", enabled)) {
				if (ImGui::InputText("Search", name_search, sizeof(name_search))) {
					kengine_logf(r, verbose, "imgui_entity_selector", "New name search entered: '%s'", name_search);
					search_out_of_date = true;
				}

				if (search_out_of_date) {
					update_search_results();
					search_out_of_date = false;
				}

				ImGui::Separator();

				ImGui::BeginChild("child");
				display_search_results();
				ImGui::EndChild();
			}
			ImGui::End();
		}

		void update_search_results() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, "imgui_entity_selector", "Updating search results");

			search_results.clear();
			r.each([&](entt::entity e) {
				apply_search(e);
			});
		}

		void apply_search(entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, "imgui_entity_selector", "Applying search to [%u]", e);

			search_result result {
				e,
				{ "[%u]", e }
			};

			const auto name = r.try_get<data::name>(e);
			if (name) {
				result.display_text += " ";
				result.display_text += name->name;
			}

			if (strlen(name_search) != 0) {
				result.display_text += " -- ";

				bool matches = false;
				if (std::isdigit(name_search[0]) && putils::parse<entt::entity>(name_search) == e) {
					kengine_log(r, very_verbose, "imgui_entity_selector", "Match found in ID");
					matches = true;
					result.display_text += "ID";
				}
				else {
					const auto types = sort_helper::get_name_sorted_entities<const meta::has, const meta::match_string>(r);

					for (const auto & [type_entity, type, has, match_func] : types) {
						if (!has->call({ r, e }) || !match_func->call({ r, e }, name_search))
							continue;

						if (result.display_text.size() + type->name.size() + 2 < decltype(result.display_text)::max_size) {
							if (matches) // Isn't the first time
								result.display_text += ", ";
							result.display_text += type->name;
						}

						kengine_logf(r, very_verbose, "imgui_entity_selector", "Match found in %s", r.get<data::name>(type_entity));
						matches = true;
					}
				}

				if (!matches) {
					kengine_log(r, very_verbose, "imgui_entity_selector", "No match found");
					return;
				}
			}

			search_results.push_back(result);
		}

		void display_search_results() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "imgui_entity_selector", "Displaying search results");

			for (const auto & result : search_results)
				if (display_search_result(result)) {
					if (r.all_of<data::selected>(result.e)) {
						kengine_logf(r, log, "imgui_entity_selector", "De-selecting [%u]", result.e);
						r.erase<data::selected>(result.e);
					}
					else {
						kengine_logf(r, log, "imgui_entity_selector", "Selecting [%u]", result.e);
						r.emplace<data::selected>(result.e);
					}
				}
		}

		bool display_search_result(const search_result & result) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, "imgui_entity_selector", "Displaying search result [%u]", result.e);

			ImGui::PushID(int(result.e));

			bool ret = false;

			const auto open_tree_node = ImGui::TreeNode(result.display_text.c_str());
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Select")) {
					kengine_logf(r, log, "imgui_entity_selector", "Toggling selection for [%u]", result.e);
					ret = true;
				}
				if (ImGui::MenuItem("Remove")) {
					kengine_logf(r, log, "imgui_entity_selector", "Destroying [%u]", result.e);
					r.destroy(result.e);
					ret = false;
				}
				ImGui::EndPopup();
			}

			if (open_tree_node) {
				imgui_helper::display_entity({ r, result.e });
				ImGui::TreePop();
			}

			ImGui::PopID();
			return ret;
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(imgui_entity_selector)
}