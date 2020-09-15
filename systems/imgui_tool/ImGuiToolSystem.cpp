#include <fstream>

#include "ImGuiToolSystem.hpp"
#include "EntityManager.hpp"

#include "helpers/sortHelper.hpp"

#include "functions/OnTerminate.hpp"
#include "functions/Execute.hpp"
#include "data/ImGuiToolComponent.hpp"
#include "data/NameComponent.hpp"
#include "imgui.h"

#include "to_string.hpp"

#ifndef KENGINE_IMGUI_TOOLS_SAVE_FILE
# define KENGINE_IMGUI_TOOLS_SAVE_FILE "tools.cnf"
#endif

#ifndef KENGINE_IMGUI_MAX_TOOLS
# define KENGINE_IMGUI_MAX_TOOLS 0 // no limit by default
#endif

namespace kengine {
#pragma region cnf file helper
	class ConfFile {
	public:
		void parse() {
			std::ifstream f(KENGINE_IMGUI_TOOLS_SAVE_FILE);
			if (!f)
				return;
			for (std::string line; std::getline(f, line);)
				addLine(line);
		}

		void addLine(const std::string & line) {
			const auto index = line.find(';');
			_values[line.substr(0, index)] = putils::parse<bool>(line.substr(index + 1).c_str());
		}

		bool getValue(const char * name) const {
			const auto it = _values.find(name);
			if (it == _values.end())
				return false;
			return it->second;
		}

	private:
		std::unordered_map<std::string, bool> _values;
	};
	static ConfFile g_confFile;
#pragma endregion cnf file helper

#pragma region declarations
	static void onEntityCreated(Entity & e);
	static void saveTools(EntityManager & em);
#pragma endregion
	EntityCreatorFunctor<64> ImGuiToolSystem(EntityManager & em) {
		g_confFile.parse();
		for (const auto & [e, name, tool] : em.getEntities<NameComponent, ImGuiToolComponent>())
			tool.enabled = g_confFile.getValue(name.name);

		return [&](Entity & e) {
			e += functions::OnEntityCreated{ onEntityCreated };
			e += functions::OnTerminate{ [&] { saveTools(em); } };

			e += functions::Execute{[&](float deltaTime) {
				if (ImGui::BeginMainMenuBar()) {
					bool mustSave = false;
					if (ImGui::BeginMenu("Tools")) {
						if (ImGui::MenuItem("Disable all"))
							for (auto & [e, tool] : em.getEntities<ImGuiToolComponent>())
								tool.enabled = false;

						const auto sorted = sortHelper::getNameSortedEntities<KENGINE_IMGUI_MAX_TOOLS, ImGuiToolComponent>(em);
						for (auto & [e, name, tool] : sorted)
							if (ImGui::MenuItem(name->name)) {
								tool->enabled = !tool->enabled;
								mustSave = true;
							}
						ImGui::EndMenu();
					}

					if (mustSave)
						saveTools(em);
				}
				ImGui::EndMainMenuBar();
			}};
		};
	}

	static void onEntityCreated(Entity & e) {
		const auto name = e.tryGet<NameComponent>();
		if (!name)
			return;

		const auto tool = e.tryGet<ImGuiToolComponent>();
		if (!tool)
			return;

		tool->enabled = g_confFile.getValue(name->name);
	}

	static void saveTools(EntityManager & em) {
		std::ofstream f(KENGINE_IMGUI_TOOLS_SAVE_FILE);
		assert(f);
		const auto sorted = sortHelper::getNameSortedEntities<KENGINE_IMGUI_MAX_TOOLS, ImGuiToolComponent>(em);
		for (const auto & [e, name, tool] : sorted)
			f << name->name << ';' << std::boolalpha << tool->enabled << std::noboolalpha << '\n';
		f.flush();
	}
}