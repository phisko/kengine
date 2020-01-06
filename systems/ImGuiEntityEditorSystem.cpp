#include "ImGuiEntityEditorSystem.hpp"
#include "EntityManager.hpp"

#include "data/ImGuiComponent.hpp"
#include "data/ImGuiToolComponent.hpp"
#include "data/SelectedComponent.hpp"
#include "data/NameComponent.hpp"

#include "helpers/TypeHelper.hpp"
#include "helpers/ImGuiHelper.hpp"
#include "imgui.h"

namespace kengine {
	EntityCreatorFunctor<64> ImGuiEntityEditorSystem(EntityManager & em) {
		return [&](Entity & e) {
			e += NameComponent{ "Entity editor" };
			auto & tool = e.attach<ImGuiToolComponent>();
			tool.enabled = true;

			e += ImGuiComponent([&] {
				if (!tool.enabled)
					return;

				for (auto & [selected, _] : em.getEntities<SelectedComponent>()) {
					bool open = true;

					ImGui::SetNextWindowSize({ 200.f, 200.f }, ImGuiCond_FirstUseEver);

					const auto beginWindow = [&] {
						if (selected.has<NameComponent>())
							return ImGui::Begin(putils::string<64>("%s##[%d]", selected.get<NameComponent>().name.c_str(), selected.id), &open, ImGuiWindowFlags_NoSavedSettings);
						else
							return ImGui::Begin(putils::string<64>("[%d] Entity editor", selected.id), nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
					};

					if (beginWindow()) {
						if (!selected.has<NameComponent>()) // no title bar
							if (ImGui::Button("x"))
								selected.detach<SelectedComponent>();

						ImGui::Separator();

						ImGui::BeginChild("##child");
						ImGuiHelper::editEntity(em, selected);
						ImGui::EndChild();
					}
					ImGui::End();

					if (!open)
						selected.detach<SelectedComponent>();
				}
			});
		};
	}
}
