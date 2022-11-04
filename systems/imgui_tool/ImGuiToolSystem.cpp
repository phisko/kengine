#include "ImGuiToolSystem.hpp"
#include "kengine.hpp"

// stl
#include <fstream>

// imgui
#include <imgui.h>

// putils
#include "to_string.hpp"

// kengine data
#include "data/ImGuiToolComponent.hpp"
#include "data/NameComponent.hpp"

// kengine functions
#include "functions/OnTerminate.hpp"
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/sortHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

#ifndef KENGINE_IMGUI_TOOLS_SAVE_FILE
# define KENGINE_IMGUI_TOOLS_SAVE_FILE "tools.cnf"
#endif

#ifndef KENGINE_IMGUI_MAX_TOOLS
# define KENGINE_IMGUI_MAX_TOOLS 0 // no limit by default
#endif

namespace kengine {
	struct ImGuiToolSystem {
		static void init(Entity & e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Log, "Init", "ImGuiToolSystem");

			e += functions::OnEntityCreated{ onEntityCreated };
			e += functions::OnTerminate{ saveTools };
			e += functions::Execute{ execute };

			_confFile.parse();
			for (const auto & [e, name, tool] : entities.with<NameComponent, ImGuiToolComponent>()) {
				kengine_logf(Log, "Init/ImGuiToolSystem", "Initializing %s", name.name.c_str());
				tool.enabled = _confFile.getValue(name.name.c_str());
			}
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Verbose, "Execute", "ImGuiToolSystem");

			if (ImGui::BeginMainMenuBar()) {
				bool mustSave = false;
				if (ImGui::BeginMenu("Tools")) {
					if (ImGui::MenuItem("Disable all")) {
						kengine_log(Log, "ImGuiToolSystem", "Disabling all tools");
						for (auto [e, tool] : entities.with<ImGuiToolComponent>())
							tool.enabled = false;
					}

					const auto sorted = sortHelper::getNameSortedEntities<KENGINE_IMGUI_MAX_TOOLS, ImGuiToolComponent>();
					for (auto & [e, name, tool] : sorted)
						if (ImGui::MenuItem(name->name.c_str())) {
							tool->enabled = !tool->enabled;
							kengine_logf(Log, "ImGuiToolSystem", "Turned %s %s", tool->enabled ? "on" : "off", name->name.c_str());
							mustSave = true;
						}
					ImGui::EndMenu();
				}

				if (mustSave)
					saveTools();
			}
			ImGui::EndMainMenuBar();
		}

		static void onEntityCreated(Entity & e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto name = e.tryGet<NameComponent>();
			if (!name)
				return;

			const auto tool = e.tryGet<ImGuiToolComponent>();
			if (!tool)
				return;

			kengine_logf(Log, "Init/ImGuiToolSystem", "Initializing %s", name->name.c_str());
			tool->enabled = _confFile.getValue(name->name.c_str());
		}

		static void saveTools() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Log, "ImGuiToolSystem", "Saving to " KENGINE_IMGUI_TOOLS_SAVE_FILE);

			std::ofstream f(KENGINE_IMGUI_TOOLS_SAVE_FILE);
			if (!f) {
				kengine_assert_failed("Failed to open '", KENGINE_IMGUI_TOOLS_SAVE_FILE, "' with write permission");
				return;
			}
			const auto sorted = sortHelper::getNameSortedEntities<KENGINE_IMGUI_MAX_TOOLS, ImGuiToolComponent>();
			for (const auto & [e, name, tool] : sorted)
				f << name->name << ';' << std::boolalpha << tool->enabled << std::noboolalpha << std::endl;
			f.flush();
		}

		static inline struct {
			void parse() noexcept {
				KENGINE_PROFILING_SCOPE;
				std::ifstream f(KENGINE_IMGUI_TOOLS_SAVE_FILE);
				if (!f)
					return;
				for (std::string line; std::getline(f, line);)
					addLine(line);
			}

			void addLine(const std::string & line) noexcept {
				KENGINE_PROFILING_SCOPE;
				const auto index = line.find(';');
				_values[line.substr(0, index)] = putils::parse<bool>(line.substr(index + 1).c_str());
			}

			bool getValue(const char * name) const noexcept {
				KENGINE_PROFILING_SCOPE;
				const auto it = _values.find(name);
				if (it == _values.end())
					return false;
				return it->second;
			}

		private:
			std::unordered_map<std::string, bool> _values;
		} _confFile;
	};

	EntityCreator * ImGuiToolSystem() noexcept {
		return ImGuiToolSystem::init;
	}
}