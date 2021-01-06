#include <fstream>

#include "ImGuiToolSystem.hpp"
#include "kengine.hpp"

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

namespace kengine::imgui_tool {
	struct impl {
		static void init(Entity & e) noexcept {
			e += functions::OnEntityCreated{ imgui_tool::impl::onEntityCreated };
			e += functions::OnTerminate{ imgui_tool::impl::saveTools };
			e += functions::Execute{ imgui_tool::impl::execute };

			imgui_tool::impl::confFile.parse();
			for (const auto & [e, name, tool] : entities.with<NameComponent, ImGuiToolComponent>())
				tool.enabled = imgui_tool::impl::confFile.getValue(name.name);
		}

		static void execute(float deltaTime) noexcept {
			if (ImGui::BeginMainMenuBar()) {
				bool mustSave = false;
				if (ImGui::BeginMenu("Tools")) {
					if (ImGui::MenuItem("Disable all"))
						for (auto [e, tool] : entities.with<ImGuiToolComponent>())
							tool.enabled = false;

					const auto sorted = sortHelper::getNameSortedEntities<KENGINE_IMGUI_MAX_TOOLS, ImGuiToolComponent>();
					for (auto & [e, name, tool] : sorted)
						if (ImGui::MenuItem(name->name)) {
							tool->enabled = !tool->enabled;
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
			const auto name = e.tryGet<NameComponent>();
			if (!name)
				return;

			const auto tool = e.tryGet<ImGuiToolComponent>();
			if (!tool)
				return;

			tool->enabled = confFile.getValue(name->name);
		}

		static void saveTools() noexcept {
			std::ofstream f(KENGINE_IMGUI_TOOLS_SAVE_FILE);
			assert(f);
			const auto sorted = sortHelper::getNameSortedEntities<KENGINE_IMGUI_MAX_TOOLS, ImGuiToolComponent>();
			for (const auto & [e, name, tool] : sorted)
				f << name->name << ';' << std::boolalpha << tool->enabled << std::noboolalpha << '\n';
			f.flush();
		}

		static inline struct {
			void parse() noexcept {
				std::ifstream f(KENGINE_IMGUI_TOOLS_SAVE_FILE);
				if (!f)
					return;
				for (std::string line; std::getline(f, line);)
					addLine(line);
			}

			void addLine(const std::string & line) noexcept {
				const auto index = line.find(';');
				_values[line.substr(0, index)] = putils::parse<bool>(line.substr(index + 1).c_str());
			}

			bool getValue(const char * name) const noexcept {
				const auto it = _values.find(name);
				if (it == _values.end())
					return false;
				return it->second;
			}

		private:
			std::unordered_map<std::string, bool> _values;
		} confFile;
	};
}

namespace kengine {
	EntityCreator * ImGuiToolSystem() noexcept {
		return [](Entity & e) noexcept {
			imgui_tool::impl::init(e);
		};
	}
}