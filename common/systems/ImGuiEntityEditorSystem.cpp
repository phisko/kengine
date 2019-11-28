#include "ImGuiEntityEditorSystem.hpp"
#include "EntityManager.hpp"

#include "components/ImGuiComponent.hpp"
#include "components/SelectedComponent.hpp"
#include "components/NameComponent.hpp"
#include "components/TypeComponent.hpp"

#include "helpers/TypeHelper.hpp"
#include "functions/Basic.hpp"
#include "functions/ImGuiEditor.hpp"

#include "helpers/ImGuiHelper.hpp"
#include "imgui.h"

auto ImGuiEntityEditor(kengine::EntityManager & em) {
	return [&](kengine::Entity & e) {
		auto & tool = e.attach<kengine::ImGuiToolComponent>();
		tool.enabled = true;
		tool.name = "Entity editor";

		e += kengine::ImGuiComponent([&] {
			if (!tool.enabled)
				return;

			for (auto &[selected, _] : em.getEntities<kengine::SelectedComponent>()) {
				bool open = true;

				ImGui::SetNextWindowSize({ 200.f, 200.f}, ImGuiCond_FirstUseEver);

				const auto beginWindow = [&] {
					if (selected.has<kengine::NameComponent>())
						return ImGui::Begin(putils::string<64>("%s##[%d]", selected.get<kengine::NameComponent>().name.c_str(), selected.id), &open, ImGuiWindowFlags_NoSavedSettings);
					else
						return ImGui::Begin(putils::string<64>("[%d] Entity editor", selected.id), nullptr, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar);
				};

				if (beginWindow()) {
					if (!selected.has<kengine::NameComponent>()) // no title bar
						if (ImGui::Button("x"))
							selected.detach<kengine::SelectedComponent>();

					ImGui::Separator();

					ImGui::BeginChild("##child");
					kengine::ImGuiHelper::editEntity(em, selected);
					ImGui::EndChild();
				}
				ImGui::End();

				if (!open)
					selected.detach<kengine::SelectedComponent>();
			}
		});
	};
}

kengine::ImGuiEntityEditorSystem::ImGuiEntityEditorSystem(kengine::EntityManager & em)
	: System(em), _em(em)
{
	_em += ImGuiEntityEditor(_em);
}
