#include "system.hpp"

// stl
#include <fstream>
#include <map>
#include <variant>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// putils
#include "putils/forward_to.hpp"
#include "putils/ini_file.hpp"
#include "putils/scn/scn.hpp"
#include "putils/split.hpp"

// kengine
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/data/name.hpp"
#include "kengine/core/helpers/new_entity_processor.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/core/sort/helpers/get_name_sorted_entities.hpp"
#include "kengine/imgui/helpers/set_context.hpp"
#include "kengine/imgui/tool/data/tool.hpp"
#include "kengine/main_loop/functions/execute.hpp"

#ifndef KENGINE_IMGUI_TOOLS_SAVE_FILE
#define KENGINE_IMGUI_TOOLS_SAVE_FILE "imgui_tools.ini"
#endif

#ifndef KENGINE_IMGUI_MAX_TOOLS
#define KENGINE_IMGUI_MAX_TOOLS 0 // no limit by default
#endif

namespace kengine::imgui::tool {
	struct system {
		static constexpr auto log_category = "imgui_tool";

		putils::ini_file configuration;
		entt::registry & r;

		struct menu_entry {
			using entry_map = std::map<std::string, menu_entry>;
			entry_map subentries;
			tool * comp = nullptr;
		};
		menu_entry root_entry;

		struct processed {};
		kengine::new_entity_processor<processed, tool> processor{ r, putils_forward_to_this(on_construct_tool) };
		const entt::scoped_connection remove_tool = r.on_destroy<tool>().connect<&system::on_destroy_imgui_tool>(this);

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));

			std::ifstream f(KENGINE_IMGUI_TOOLS_SAVE_FILE);
			f >> configuration;

			processor.process();
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");

			if (!imgui::set_context(r))
				return;

			processor.process();

			if (ImGui::BeginMainMenuBar()) {
				bool must_save = false;
				if (ImGui::BeginMenu("Tools")) {
					if (ImGui::MenuItem("Disable all")) {
						kengine_log(r, log, log_category, "Disabling all tools");
						for (auto [e, comp] : r.view<tool>().each()) {
							kengine_logf(r, verbose, log_category, "Disabling {}", r.get<core::name>(e).name);
							comp.enabled = false;
						}
					}

					for (const auto & [name, entry] : root_entry.subentries)
						display_menu_entry(name.c_str(), entry, must_save);
					ImGui::EndMenu();
				}

				if (must_save)
					save_tools();
			}
			ImGui::EndMainMenuBar();
		}

		void display_menu_entry(const char * name, const menu_entry & entry, bool & must_save) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, log_category, "Displaying menu entry {}", name);

			if (entry.comp) {
				if (ImGui::MenuItem(name, nullptr, entry.comp->enabled)) {
					entry.comp->enabled = !entry.comp->enabled;
					kengine_logf(r, log, log_category, "Turned {} {}", entry.comp->enabled ? "on" : "off", name);
					must_save = true;
				}
				return;
			}

			if (ImGui::BeginMenu(name)) {
				for (const auto & [subentry_name, subentry] : entry.subentries)
					display_menu_entry(subentry_name.c_str(), subentry, must_save);
				ImGui::EndMenu();
			}
		}

		void on_construct_tool(entt::entity e, tool & comp) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto name = r.try_get<core::name>(e);
			if (!name) {
				kengine_assert_failed(r, "{} has a imgui::tool::tool but no core::name", e);
				return;
			}

			kengine_logf(r, verbose, log_category, "Initializing {}", name->name);
			const auto & section = configuration.sections["Tools"];
			if (const auto it = section.values.find(name->name.c_str()); it != section.values.end()) {
				const auto result = scn::scan_default(it->second, comp.enabled);
				if (!result)
					kengine_assert_failed(r, "{}", result.error().msg());
			}

			menu_entry * current_entry = &root_entry;
			const auto entry_names = putils::split(name->name, '/');
			for (const auto entry_name : entry_names) {
				kengine_assert_with_message(r, current_entry->comp == nullptr, "Intermediate menu '{}' for ImGui tool '{}' has the name of an existing tool", entry_name, name->name);
				current_entry = &current_entry->subentries[std::string(entry_name)];
			}

			kengine_assert_with_message(r, current_entry->subentries.empty(), "Leaf entry for ImGui tool '{}' was previously used as an intermediate menu", name->name);
			current_entry->comp = &comp;
		}

		void on_destroy_imgui_tool(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Destroyed ImGui tool in {}", e);

			const auto & comp = r.get<tool>(e);
			if (!remove_tool_from_entry(comp, root_entry))
				kengine_assert_failed(r, "Removed ImGui tool was not found in map");
		}

		bool remove_tool_from_entry(const tool & comp, menu_entry & entry) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (entry.comp == &comp)
				return true;

			for (auto it = entry.subentries.begin(); it != entry.subentries.end(); ++it) {
				auto & subentry = it->second;
				if (remove_tool_from_entry(comp, subentry)) {
					if (subentry.subentries.empty())
						entry.subentries.erase(it);
					return true;
				}
			}

			return false;
		}

		void save_tools() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Saving to " KENGINE_IMGUI_TOOLS_SAVE_FILE);

			std::ofstream f(KENGINE_IMGUI_TOOLS_SAVE_FILE);
			if (!f) {
				kengine_assert_failed(r, "Failed to open '" KENGINE_IMGUI_TOOLS_SAVE_FILE "' with write permission");
				return;
			}

			putils::ini_file output_configuration;
			auto & section = output_configuration.sections["Tools"];

			const auto sorted = core::sort::get_name_sorted_entities<KENGINE_IMGUI_MAX_TOOLS, tool>(r);
			for (const auto & [e, name, comp] : sorted) {
				auto value = fmt::format("{}", comp->enabled);
				kengine_logf(r, verbose, log_category, "Saving {} ({})", name->name, value);
				section.values[name->name.c_str()] = std::move(value);
			}

			f << output_configuration;
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		system,
		system::processed
	)
}