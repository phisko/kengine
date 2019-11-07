#include "ImGuiToolSystem.hpp"
#include "EntityManager.hpp"

#include "components/ImGuiComponent.hpp"
#include "imgui.h"

#include "to_string.hpp"

#ifndef KENGINE_IMGUI_TOOLS_SAVE_FILE
# define KENGINE_IMGUI_TOOLS_SAVE_FILE "tools.cnf"
#endif

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

static void saveTools(kengine::EntityManager & em) {
	std::ofstream f(KENGINE_IMGUI_TOOLS_SAVE_FILE);
	assert(f);
	for (const auto & [e, tool] : em.getEntities<kengine::ImGuiToolComponent>())
		f << tool.name << ';' << std::boolalpha << tool.enabled << std::noboolalpha << '\n';
	f.flush();
}

static auto ToolsController(kengine::EntityManager & em) {
	return [&](kengine::Entity & e) {
		e += kengine::ImGuiComponent([&] {
			if (ImGui::BeginMainMenuBar()) {
				bool mustSave = false;
				if (ImGui::BeginMenu("Tools")) {
					if (ImGui::MenuItem("Disable all"))
						for (auto & [e, tool] : em.getEntities<kengine::ImGuiToolComponent>())
							tool.enabled = false;

					for (auto & [e, tool] : em.getEntities<kengine::ImGuiToolComponent>())
						if (ImGui::MenuItem(tool.name)) {
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

namespace kengine {
	ImGuiToolSystem::ImGuiToolSystem(EntityManager & em) : System(em), _em(em) {
		onLoad("");
		g_confFile.parse();
	}

	void ImGuiToolSystem::onLoad(const char *) noexcept {
		_em += ToolsController(_em);
	}

	void ImGuiToolSystem::handle(packets::RegisterEntity p) noexcept {
		if (!p.e.has<ImGuiToolComponent>())
			return;

		auto & tool = p.e.get<ImGuiToolComponent>();
		tool.enabled = g_confFile.getValue(tool.name);
	}
}