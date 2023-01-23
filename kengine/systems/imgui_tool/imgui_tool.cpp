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
#define KENGINE_IMGUI_TOOLS_SAVE_FILE "tools.cnf"
#endif

#ifndef KENGINE_IMGUI_MAX_TOOLS
#define KENGINE_IMGUI_MAX_TOOLS 0 // no limit by default
#endif

namespace kengine::systems {
	struct imgui_tool {
		entt::registry & r;
		putils::vector<entt::scoped_connection, 2> connections;

		struct menu_entry {
			using entry_map = std::map<std::string, menu_entry>;
			entry_map subentries;
			data::imgui_tool * tool = nullptr;
		};
		menu_entry root_entry;

		imgui_tool(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "Init", "systems/imgui_tool");

			conf_file.parse();
			for (const auto & [tool_entity, name, tool] : r.view<data::name, data::imgui_tool>().each()) {
				kengine_logf(r, log, "Init/systems/imgui_tool", "Initializing %s", name.name.c_str());
				if (const auto value = conf_file.get_value(name.name.c_str()))
					tool.enabled = *value;
			}

			e.emplace<functions::execute>(putils_forward_to_this(execute));

			for (const auto tool_entity : r.view<data::imgui_tool>())
				on_construct_imgui_tool(r, tool_entity);

			connections.emplace_back(r.on_construct<data::imgui_tool>().connect<&imgui_tool::on_construct_imgui_tool>(this));
			connections.emplace_back(r.on_destroy<data::imgui_tool>().connect<&imgui_tool::on_destroy_imgui_tool>(this));
		}

		~imgui_tool() noexcept {
			KENGINE_PROFILING_SCOPE;
			save_tools();
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, "execute", "imgui_tool");

			if (ImGui::BeginMainMenuBar()) {
				bool must_save = false;
				if (ImGui::BeginMenu("Tools")) {
					if (ImGui::MenuItem("Disable all")) {
						kengine_log(r, log, "imgui_tool", "Disabling all tools");
						for (auto [e, tool] : r.view<data::imgui_tool>().each())
							tool.enabled = false;
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

		void on_construct_imgui_tool(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto name = r.try_get<data::name>(e);
			if (!name) {
				kengine_assert_failed(r, "Entity ", int(e), " has a data::imgui_tool but no data::name");
				return;
			}

			auto & tool = r.get<data::imgui_tool>(e);
			kengine_logf(r, log, "Init/systems/imgui_tool", "Initializing %s", name->name.c_str());
			if (const auto value = conf_file.get_value(name->name.c_str()))
				tool.enabled = *value;

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
			const auto sorted = sort_helper::get_name_sorted_entities<KENGINE_IMGUI_MAX_TOOLS, data::imgui_tool>(r);
			for (const auto & [e, name, tool] : sorted)
				f << name->name << ';' << std::boolalpha << tool->enabled << std::noboolalpha << std::endl;
			f.flush();
		}

		struct {
			void parse() noexcept {
				KENGINE_PROFILING_SCOPE;
				std::ifstream f(KENGINE_IMGUI_TOOLS_SAVE_FILE);
				if (!f)
					return;
				for (std::string line; std::getline(f, line);)
					add_line(line);
			}

			void add_line(const std::string & line) noexcept {
				KENGINE_PROFILING_SCOPE;
				const auto index = line.find(';');
				_values[line.substr(0, index)] = putils::parse<bool>(line.substr(index + 1).c_str());
			}

			std::optional<bool> get_value(const char * name) const noexcept {
				KENGINE_PROFILING_SCOPE;
				const auto it = _values.find(name);
				if (it == _values.end())
					return std::nullopt;
				return it->second;
			}

		private:
			std::unordered_map<std::string, bool> _values;
		} conf_file;
	};

	entt::entity add_imgui_tool(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<imgui_tool>(e);
		return e;
	}
}