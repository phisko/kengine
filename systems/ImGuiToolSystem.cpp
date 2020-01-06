#include "ImGuiToolSystem.hpp"
#include "EntityManager.hpp"

#include "functions/OnTerminate.hpp"
#include "data/ImGuiComponent.hpp"
#include "data/ImGuiToolComponent.hpp"
#include "data/NameComponent.hpp"
#include "imgui.h"

#include "to_string.hpp"

#ifndef KENGINE_IMGUI_TOOLS_SAVE_FILE
# define KENGINE_IMGUI_TOOLS_SAVE_FILE "tools.cnf"
#endif

namespace kengine {
	// cnf file helper
	class ConfFile {
	public:
		void parse();
		void addLine(const std::string & line);
		bool getValue(const char * name) const;

	private:
		std::unordered_map<std::string, bool> _values;
	};
	static ConfFile g_confFile;
	//

	// declarations
	static void onEntityCreated(Entity & e);
	static void saveTools(kengine::EntityManager & em);
	//
	EntityCreatorFunctor<64> ImGuiToolSystem(EntityManager & em) {
		g_confFile.parse();
		for (const auto & [e, name, tool] : em.getEntities<NameComponent, ImGuiToolComponent>())
			tool.enabled = g_confFile.getValue(name.name);

		return [&](Entity & e) {
			e += functions::OnEntityCreated{ onEntityCreated };
			e += functions::OnTerminate{ [&] { saveTools(em); } };

			e += kengine::ImGuiComponent([&] {
				if (ImGui::BeginMainMenuBar()) {
					bool mustSave = false;
					if (ImGui::BeginMenu("Tools")) {
						if (ImGui::MenuItem("Disable all"))
							for (auto & [e, tool] : em.getEntities<ImGuiToolComponent>())
								tool.enabled = false;

						for (auto & [e, name, tool] : em.getEntities<NameComponent, ImGuiToolComponent>())
							if (ImGui::MenuItem(name.name)) {
								tool.enabled = !tool.enabled;
								mustSave = true;
							}
						ImGui::EndMenu();
					}

					if (mustSave)
						saveTools(em);
				}
				ImGui::EndMainMenuBar();
			});
		};
	}

	static void onEntityCreated(Entity & e) {
		if (!e.has<ImGuiToolComponent>() || !e.has<NameComponent>())
			return;

		const auto & name = e.get<NameComponent>();
		auto & tool = e.get<ImGuiToolComponent>();
		tool.enabled = g_confFile.getValue(name.name);
	}

	static void saveTools(kengine::EntityManager & em) {
		std::ofstream f(KENGINE_IMGUI_TOOLS_SAVE_FILE);
		assert(f);
		for (const auto & [e, name, tool] : em.getEntities<kengine::NameComponent, kengine::ImGuiToolComponent>())
			f << name.name << ';' << std::boolalpha << tool.enabled << std::noboolalpha << '\n';
		f.flush();
	}

	void ConfFile::parse() {
		std::ifstream f(KENGINE_IMGUI_TOOLS_SAVE_FILE);
		if (!f)
			return;
		for (std::string line; std::getline(f, line);)
			addLine(line);
	}

	void ConfFile::addLine(const std::string & line) {
		const auto index = line.find(';');
		_values[line.substr(0, index)] = putils::parse<bool>(line.substr(index + 1).c_str());
	}

	bool ConfFile::getValue(const char * name) const {
		const auto it = _values.find(name);
		if (it == _values.end())
			return false;
		return it->second;
	}
}