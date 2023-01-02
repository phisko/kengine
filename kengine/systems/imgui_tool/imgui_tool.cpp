#include "imgui_tool.hpp"

// stl
#include <fstream>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// putils
#include "putils/forward_to.hpp"
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
		entt::scoped_connection connection;

		imgui_tool(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "Init", "systems/imgui_tool");

			conf_file.parse();
			for (const auto & [tool_entity, name, tool] : r.view<data::name, data::imgui_tool>().each()) {
				kengine_logf(r, log, "Init/systems/imgui_tool", "Initializing %s", name.name.c_str());
				tool.enabled = conf_file.get_value(name.name.c_str());
			}

			e.emplace<functions::execute>(putils_forward_to_this(execute));

			connection = r.on_construct<data::imgui_tool>().connect<&imgui_tool::on_construct_imgui_tool>(this);
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

					const auto sorted = sort_helper::get_name_sorted_entities<KENGINE_IMGUI_MAX_TOOLS, data::imgui_tool>(r);
					for (auto & [e, name, tool] : sorted)
						if (ImGui::MenuItem(name->name.c_str())) {
							tool->enabled = !tool->enabled;
							kengine_logf(r, log, "imgui_tool", "Turned %s %s", tool->enabled ? "on" : "off", name->name.c_str());
							must_save = true;
						}
					ImGui::EndMenu();
				}

				if (must_save)
					save_tools();
			}
			ImGui::EndMainMenuBar();
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
			tool.enabled = conf_file.get_value(name->name.c_str());
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

			bool get_value(const char * name) const noexcept {
				KENGINE_PROFILING_SCOPE;
				const auto it = _values.find(name);
				if (it == _values.end())
					return false;
				return it->second;
			}

		private:
			std::unordered_map<std::string, bool> _values;
		} conf_file;
	};

	void add_imgui_tool(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<imgui_tool>(e);
	}
}