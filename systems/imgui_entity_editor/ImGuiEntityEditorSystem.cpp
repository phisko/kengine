#include "ImGuiEntityEditorSystem.hpp"
#include "kengine.hpp"

#include "data/ImGuiToolComponent.hpp"
#include "data/SelectedComponent.hpp"
#include "data/NameComponent.hpp"

#include "functions/Execute.hpp"
#include "functions/GetImGuiScale.hpp"

#include "helpers/typeHelper.hpp"
#include "helpers/imGuiHelper.hpp"
#include "imgui.h"

namespace kengine::imgui_entity_editor {
	struct impl {
		static inline bool * enabled;

		static void init(Entity & e) noexcept {
			e += NameComponent{ "Entity editor" };
			auto & tool = e.attach<ImGuiToolComponent>();
			tool.enabled = true;
			enabled = &tool.enabled;

			e += functions::Execute{ execute };
		}

		static void execute(float deltaTime) noexcept {
			if (!*enabled)
				return;

			float scale = 1.f;
			for (const auto & [e, getScale] : entities.with<functions::GetImGuiScale>())
				scale = getScale();

			for (auto [selected, _] : entities.with<SelectedComponent>()) {
				bool open = true;

				ImGui::SetNextWindowSize({ 200.f * scale, 200.f * scale }, ImGuiCond_FirstUseEver);

				const auto beginWindow = [&selected, &open] {
					const auto name = selected.tryGet<NameComponent>();
					if (name)
						return ImGui::Begin(putils::string<64>("%s##[%d]", name->name.c_str(), selected.id), &open, ImGuiWindowFlags_NoSavedSettings);
					else
						return ImGui::Begin(putils::string<64>("[%d] Entity editor", selected.id), nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
				};

				if (beginWindow()) {
					if (!selected.has<NameComponent>()) { // no title bar
						if (ImGui::Button("x"))
							selected.detach<SelectedComponent>();
						ImGui::Separator();
					}

					ImGui::BeginChild("##child");
					imguiHelper::editEntity(selected);
					ImGui::EndChild();
				}
				ImGui::End();

				if (!open)
					selected.detach<SelectedComponent>();
			}
		}
	};
}

namespace kengine {
	EntityCreator * ImGuiEntityEditorSystem() noexcept {
		return [](Entity & e) noexcept {
			imgui_entity_editor::impl::init(e);
		};
	}
}
