#include "system.hpp"

// stl
#include <map>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// putils
#include "putils/forward_to.hpp"
#include "putils/split.hpp"

// kengine
#include "kengine/config/data/configurable.hpp"
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/data/name.hpp"
#include "kengine/core/helpers/new_entity_processor.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/imgui/helpers/set_context.hpp"
#include "kengine/imgui/tool/data/tool.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/meta/functions/has.hpp"
#include "kengine/meta/functions/has_metadata.hpp"
#include "kengine/meta/imgui/functions/edit.hpp"
#include "kengine/meta/json/helpers/save_entity.hpp"

namespace kengine::config::imgui {
	static constexpr auto log_category = "config_imgui";
	static constexpr auto config_metadata = "config";

	struct system {
		entt::registry & r;

		bool * enabled;

		struct section {
			using section_map = std::map<std::string, section>;
			section_map subsections;

			struct entry {
				entt::entity e = entt::null;
				bool passes_search = true;
			};

			std::vector<entry> entries;
			bool passes_search = true;
		};
		section root_section;

		// Remove section from ImGui tree when config is destroyed
		const entt::scoped_connection connection = r.on_destroy<configurable>().connect<&system::on_destroy_config>(this);

		// Add new section to ImGui tree when config is created
		struct processed {};
		kengine::new_entity_processor<processed, core::name, configurable> processor{ r, putils_forward_to_this(on_construct_config) };

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));

			e.emplace<core::name>("Config values");
			auto & tool = e.emplace<kengine::imgui::tool::tool>();
			enabled = &tool.enabled;

			processor.process();
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

			processor.process();

			if (ImGui::Begin("Config values", enabled)) {
				if (ImGui::InputText("Name", name_search, sizeof(name_search))) {
					kengine_logf(r, verbose, log_category, "Name search changed to '{}'", name_search);
					search_out_of_date = true;
				}
				ImGui::Separator();

				if (search_out_of_date) {
					kengine_log(r, verbose, log_category, "Updating search results");
					update_search_results("", root_section);
					search_out_of_date = false;
				}

				if (ImGui::BeginChild("##config values"))
					for (const auto & [name, section] : root_section.subsections)
						display_menu_entry(name.c_str(), section);
				ImGui::EndChild();
			}
			ImGui::End();
		}

		void update_search_results(const std::string & name, section & section) noexcept {
			KENGINE_PROFILING_SCOPE;

			section.passes_search = false;

			for (auto & [subentry_name, subsection] : section.subsections) {
				update_search_results(name + '/' + subentry_name, subsection);
				if (subsection.passes_search)
					section.passes_search = true;
			}

			for (auto & entry : section.entries) {
				entry.passes_search = false;

				const auto str = meta::json::save_entity({ r, entry.e }).dump();
				if (str.find(name_search) != std::string::npos) {
					entry.passes_search = true;
					section.passes_search = true;
				}
			}
		}

		void display_menu_entry(const char * name, const section & section) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!section.passes_search)
				return;

			if (ImGui::TreeNodeEx(name)) {
				for (const auto & entry : section.entries)
					if (entry.passes_search)
						draw(entry.e);

				for (const auto & [subsection_name, subsection] : section.subsections)
					display_menu_entry(subsection_name.c_str(), subsection);

				ImGui::TreePop();
			}
		}

		void draw(entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			for (const auto & [type_entity, has, imgui_edit, has_metadata] : r.view<meta::has, meta::imgui::edit, meta::has_metadata>().each())
				if (has({ r, e }) && has_metadata(config_metadata))
					if (imgui_edit({ r, e }))
						r.patch<configurable>(e);
		}

		void on_construct_config(entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto & name = r.get<core::name>(e);
			const auto section_names = putils::split(name.name, '/');

			section * current_section = &root_section;
			for (const auto section_name : section_names)
				current_section = &current_section->subsections[std::string(section_name)];

			section::entry entry;
			entry.e = e;
			current_section->entries.push_back(entry);

			search_out_of_date = true;
		}

		void on_destroy_config(entt::registry &, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			kengine_logf(r, verbose, log_category, "Config destroyed in {}", e);
			remove_config_from_section(e, root_section);
		}

		bool remove_config_from_section(entt::entity e, section & section) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (const auto it = std::ranges::find_if(section.entries, [&](const auto & entry) { return entry.e == e; }); it != section.entries.end()) {
				section.entries.erase(it);
				return true;
			}

			for (auto it = section.subsections.begin(); it != section.subsections.end(); ++it) {
				auto & subsection = it->second;
				if (remove_config_from_section(e, subsection)) {
					if (subsection.subsections.empty())
						section.subsections.erase(it);
					return true;
				}
			}

			search_out_of_date = true;
			return false;
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		system,
		system::processed
	)
}
