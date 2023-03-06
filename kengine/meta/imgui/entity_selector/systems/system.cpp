#include "system.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// putils
#include "putils/forward_to.hpp"
#include "putils/string.hpp"

// kengine
#include "kengine/core/data/name.hpp"
#include "kengine/core/data/selected.hpp"
#include "kengine/core/helpers/entt_scanner.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/core/sort/helpers/get_name_sorted_entities.hpp"
#include "kengine/imgui/helpers/set_context.hpp"
#include "kengine/imgui/tool/data/tool.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/meta/functions/has.hpp"
#include "kengine/meta/functions/match_string.hpp"
#include "kengine/meta/imgui/helpers/display_entity.hpp"

namespace kengine::meta::imgui::entity_selector {
	static constexpr auto log_category = "meta_imgui_entity_selector";

	struct system {
		entt::registry & r;
		bool * enabled;

		struct search_result {
			entt::entity e;
			putils::string<1024> display_text;
		};
		std::vector<search_result> search_results;

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));

			e.emplace<core::name>("Entities/Selector");
			auto & tool = e.emplace<kengine::imgui::tool::tool>();
			enabled = &tool.enabled;
		}

		char name_search[1024] = "";
		bool search_out_of_date = true;
		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");

			if (!*enabled) {
				kengine_log(r, very_verbose, log_category, "Disabled");
				return;
			}

			if (!kengine::imgui::set_context(r))
				return;

			if (ImGui::Begin("Entity selector", enabled)) {
				if (ImGui::InputText("Search", name_search, sizeof(name_search))) {
					kengine_logf(r, verbose, log_category, "New name search entered: '{}'", name_search);
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
			kengine_log(r, verbose, log_category, "Updating search results");

			search_results.clear();
			r.each([&](entt::entity e) {
				apply_search(e);
			});
		}

		void apply_search(entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Applying search to {}", e);

			search_result result{
				e,
				{ "{}", e }
			};

			const auto name = r.try_get<core::name>(e);
			if (name) {
				result.display_text += " ";
				result.display_text += name->name;
			}

			if (strlen(name_search) != 0) {
				result.display_text += " -- ";

				bool matches = false;

				entt::entity id;
				if (scn::scan_default(name_search, id) && id == e) {
					kengine_log(r, very_verbose, log_category, "Match found in ID");
					matches = true;
					result.display_text += "ID";
				}
				else {
					const auto types = core::sort::get_name_sorted_entities<const meta::has, const meta::match_string>(r);

					for (const auto & [type_entity, type, has, match_func] : types) {
						if (!has->call({ r, e }) || !match_func->call({ r, e }, name_search))
							continue;

						if (result.display_text.size() + type->name.size() + 2 < result.display_text.max_size()) {
							if (matches) // Isn't the first time
								result.display_text += ", ";
							result.display_text += type->name;
						}

						kengine_logf(r, very_verbose, log_category, "Match found in {}", r.get<core::name>(type_entity));
						matches = true;
					}
				}

				if (!matches) {
					kengine_log(r, very_verbose, log_category, "No match found");
					return;
				}
			}

			search_results.push_back(result);
		}

		void display_search_results() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Displaying search results");

			for (const auto & result : search_results)
				if (display_search_result(result)) {
					if (r.all_of<core::selected>(result.e)) {
						kengine_logf(r, log, log_category, "De-selecting {}", result.e);
						r.erase<core::selected>(result.e);
					}
					else {
						kengine_logf(r, log, log_category, "Selecting {}", result.e);
						r.emplace<core::selected>(result.e);
					}
				}
		}

		bool display_search_result(const search_result & result) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Displaying search result {}", result.e);

			ImGui::PushID(int(result.e));

			bool ret = false;

			const auto open_tree_node = ImGui::TreeNode(result.display_text.c_str());
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Select")) {
					kengine_logf(r, log, log_category, "Toggling selection for {}", result.e);
					ret = true;
				}
				if (ImGui::MenuItem("Remove")) {
					kengine_logf(r, log, log_category, "Destroying {}", result.e);
					r.destroy(result.e);
					ret = false;
				}
				ImGui::EndPopup();
			}

			if (open_tree_node) {
				display_entity({ r, result.e });
				ImGui::TreePop();
			}

			ImGui::PopID();
			return ret;
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system)
}