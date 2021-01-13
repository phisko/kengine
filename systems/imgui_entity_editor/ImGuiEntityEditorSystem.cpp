#include "ImGuiEntityEditorSystem.hpp"
#include "kengine.hpp"

#include "data/ImGuiToolComponent.hpp"
#include "data/SelectedComponent.hpp"
#include "data/NameComponent.hpp"

#include "functions/Execute.hpp"

#include "helpers/typeHelper.hpp"
#include "helpers/imGuiHelper.hpp"
#include "imgui.h"

namespace kengine {
	EntityCreator * ImGuiEntityEditorSystem() noexcept {
		static bool * enabled;

		struct impl {
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

				const auto scale = imguiHelper::getScale();
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

		return impl::init;
	}
}
