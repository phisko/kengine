#include "imgui_tool.hpp"

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
#include "putils/split.hpp"
#include "putils/to_string.hpp"

// kengine data
#include "kengine/data/imgui_tool.hpp"
#include "kengine/data/name.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/assert_helper.hpp"
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"
#include "kengine/helpers/sort_helper.hpp"

#ifndef KENGINE_IMGUI_TOOLS_SAVE_FILE
#define KENGINE_IMGUI_TOOLS_SAVE_FILE "imgui_tools.ini"
#endif

#ifndef KENGINE_IMGUI_MAX_TOOLS
#define KENGINE_IMGUI_MAX_TOOLS 0 // no limit by default
#endif

namespace kengine::systems {
	struct imgui_tool {
		putils::ini_file configuration;

		entt::registry & r;
		const entt::scoped_connection remove_tool = r.on_destroy<data::imgui_tool>().connect<&imgui_tool::on_destroy_imgui_tool>(this);

		struct menu_entry {
			using entry_map = std::map<std::string, menu_entry>;
			entry_map subentries;
			data::imgui_tool * tool = nullptr;
		};
		menu_entry root_entry;

		imgui_tool(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "imgui_tool", "Initializing");

			e.emplace<functions::execute>(putils_forward_to_this(execute));

			std::ifstream f(KENGINE_IMGUI_TOOLS_SAVE_FILE);
			f >> configuration;

			process_new_entities();
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "imgui_tool", "Executing");

			process_new_entities();

			if (ImGui::BeginMainMenuBar()) {
				bool must_save = false;
				if (ImGui::BeginMenu("Tools")) {
					if (ImGui::MenuItem("Disable all")) {
						kengine_log(r, log, "imgui_tool", "Disabling all tools");
						for (auto [e, tool] : r.view<data::imgui_tool>().each()) {
							kengine_logf(r, verbose, "imgui_tool", "Disabling %s", r.get<data::name>(e));
							tool.enabled = false;
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

		struct processed {};
		void process_new_entities() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "imgui_tool", "Processing new entities");

			for (const auto & [e, imgui_tool] : r.view<data::imgui_tool>(entt::exclude<processed>).each()) {
				r.emplace<processed>(e);
				on_construct_imgui_tool(e, imgui_tool);
			}
		}

		void display_menu_entry(const char * name, const menu_entry & entry, bool & must_save) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, very_verbose, "imgui_tool", "Displaying menu entry %s", name);

			if (entry.tool) {
				if (ImGui::MenuItem(name, nullptr, entry.tool->enabled)) {
					entry.tool->enabled = !entry.tool->enabled;
					kengine_logf(r, log, "imgui_tool", "Turned %s %s", entry.tool->enabled ? "on" : "off", name);
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

		void on_construct_imgui_tool(entt::entity e, data::imgui_tool & tool) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto name = r.try_get<data::name>(e);
			if (!name) {
				kengine_assert_failed(r, "Entity ", int(e), " has a data::imgui_tool but no data::name");
				return;
			}

			kengine_logf(r, verbose, "imgui_tool", "Initializing %s", name->name.c_str());
			const auto & section = configuration.sections["Tools"];
			if (const auto it = section.values.find(name->name.c_str()); it != section.values.end())
				tool.enabled = putils::parse<bool>(it->second);

			menu_entry * current_entry = &root_entry;
			const auto entry_names = putils::split(name->name.c_str(), '/');
			for (const auto & entry_name : entry_names) {
				kengine_assert_with_message(r, current_entry->tool == nullptr, "Intermediate menu '", entry_name, "' for ImGui tool '", name->name, "' has the name of an existing tool");
				current_entry = &current_entry->subentries[entry_name];
			}

			kengine_assert_with_message(r, current_entry->subentries.empty(), "Leaf entry for ImGui tool '", name->name, "' was previously used as an intermediate menu");
			current_entry->tool = &tool;
		}

		void on_destroy_imgui_tool(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, "imgui_tool", "Destroyed ImGui tool in [%u]", e);

			const auto & tool = r.get<data::imgui_tool>(e);
			if (!remove_tool_from_entry(tool, root_entry))
				kengine_assert_failed(r, "Removed ImGui tool was not found in map");
		}

		bool remove_tool_from_entry(const data::imgui_tool & tool, menu_entry & entry) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (entry.tool == &tool)
				return true;

			for (auto it = entry.subentries.begin(); it != entry.subentries.end(); ++it) {
				auto & subentry = it->second;
				if (remove_tool_from_entry(tool, subentry)) {
					if (subentry.subentries.empty())
						entry.subentries.erase(it);
					return true;
				}
			}

			return false;
		}

		void save_tools() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "imgui_tool", "Saving to " KENGINE_IMGUI_TOOLS_SAVE_FILE);

			std::ofstream f(KENGINE_IMGUI_TOOLS_SAVE_FILE);
			if (!f) {
				kengine_assert_failed(r, "Failed to open '", KENGINE_IMGUI_TOOLS_SAVE_FILE, "' with write permission");
				return;
			}

			putils::ini_file output_configuration;
			auto & section = output_configuration.sections["Tools"];

			const auto sorted = sort_helper::get_name_sorted_entities<KENGINE_IMGUI_MAX_TOOLS, data::imgui_tool>(r);
			for (const auto & [e, name, tool] : sorted) {
				std::string value = putils::to_string(tool->enabled);
				kengine_logf(r, verbose, "imgui_tool", "Saving %s (%s)", name->name.c_str(), value.c_str());
				section.values[name->name.c_str()] = std::move(value);
			}

			f << output_configuration;
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		imgui_tool,
		imgui_tool::processed
	)
}