#include "ImGuiToolSystem.hpp"

// stl
#include <fstream>

// entt
#include <entt/entity/registry.hpp>

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
#include "helpers/assertHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"
#include "helpers/sortHelper.hpp"

#ifndef KENGINE_IMGUI_TOOLS_SAVE_FILE
# define KENGINE_IMGUI_TOOLS_SAVE_FILE "tools.cnf"
#endif

#ifndef KENGINE_IMGUI_MAX_TOOLS
# define KENGINE_IMGUI_MAX_TOOLS 0 // no limit by default
#endif

namespace kengine {
	struct ImGuiToolSystem {
		static void init(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "ImGuiToolSystem");

			_r = &r;

			r.on_construct<ImGuiToolComponent>().connect<onConstructImGuiTool>();

			const auto e = r.create();
			r.emplace<functions::OnTerminate>(e, saveTools);
			r.emplace<functions::Execute>(e, execute);

			_confFile.parse();
			for (const auto & [toolEntity, name, tool] : r.view<NameComponent, ImGuiToolComponent>().each()) {
				kengine_logf(r, Log, "Init/ImGuiToolSystem", "Initializing %s", name.name.c_str());
				tool.enabled = _confFile.getValue(name.name.c_str());
			}
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(*_r, Verbose, "Execute", "ImGuiToolSystem");

			if (ImGui::BeginMainMenuBar()) {
				bool mustSave = false;
				if (ImGui::BeginMenu("Tools")) {
					if (ImGui::MenuItem("Disable all")) {
						kengine_log(*_r, Log, "ImGuiToolSystem", "Disabling all tools");
						for (auto [e, tool] : _r->view<ImGuiToolComponent>().each())
							tool.enabled = false;
					}

					const auto sorted = sortHelper::getNameSortedEntities<KENGINE_IMGUI_MAX_TOOLS, ImGuiToolComponent>(*_r);
					for (auto & [e, name, tool] : sorted)
						if (ImGui::MenuItem(name->name.c_str())) {
							tool->enabled = !tool->enabled;
							kengine_logf(*_r, Log, "ImGuiToolSystem", "Turned %s %s", tool->enabled ? "on" : "off", name->name.c_str());
							mustSave = true;
						}
					ImGui::EndMenu();
				}

				if (mustSave)
					saveTools();
			}
			ImGui::EndMainMenuBar();
		}

		static void onConstructImGuiTool(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto name = r.try_get<NameComponent>(e);
			if (!name) {
				kengine_assert_failed(r, "Entity ", int(e), " has a ImGuiToolComponent but no NameComponent");
				return;
			}

			auto & tool = r.get<ImGuiToolComponent>(e);
			kengine_logf(r, Log, "Init/ImGuiToolSystem", "Initializing %s", name->name.c_str());
			tool.enabled = _confFile.getValue(name->name.c_str());
		}

		static void saveTools() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(*_r, Log, "ImGuiToolSystem", "Saving to " KENGINE_IMGUI_TOOLS_SAVE_FILE);

			std::ofstream f(KENGINE_IMGUI_TOOLS_SAVE_FILE);
			if (!f) {
				kengine_assert_failed(*_r, "Failed to open '", KENGINE_IMGUI_TOOLS_SAVE_FILE, "' with write permission");
				return;
			}
			const auto sorted = sortHelper::getNameSortedEntities<KENGINE_IMGUI_MAX_TOOLS, ImGuiToolComponent>(*_r);
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

		static inline entt::registry * _r;
	};

	void ImGuiToolSystem(entt::registry & r) noexcept {
		ImGuiToolSystem::init(r);
	}
}