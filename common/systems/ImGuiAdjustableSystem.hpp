#pragma once

#include "System.hpp"
#include "components/AdjustableComponent.hpp"
#include "components/ImGuiComponent.hpp"
#include "imgui.h"

namespace kengine {
	class ImGuiAdjustableSystem : public kengine::System<ImGuiAdjustableSystem> {
	public:
		ImGuiAdjustableSystem(kengine::EntityManager & em) {
			kengine::ImGuiComponent::create("imgui-adjustables", em, [&em] {
				if (ImGui::Begin("Adjustables")) {
					for (const auto go : em.getGameObjects<AdjustableComponent>()) {
						auto & comp = go->getComponent<AdjustableComponent>();

						if (comp.adjustableType == AdjustableComponent::String) {
							char buff[1024];
							comp.s.copy(buff, sizeof(buff));
							ImGui::InputText(comp.name.c_str(), buff, sizeof(buff));
							comp.s = buff;
							if (comp.sPtr != nullptr)
								*comp.sPtr = comp.s;
						}
						else if (comp.adjustableType == AdjustableComponent::Bool) {
							ImGui::Checkbox(comp.name.c_str(), &comp.b);
							if (comp.bPtr != nullptr)
								*comp.bPtr = comp.b;
						}
						else if (comp.adjustableType == AdjustableComponent::Double) {
							ImGui::InputDouble(comp.name.c_str(), &comp.d);
							if (comp.dPtr != nullptr)
								*comp.dPtr = comp.d;
						}
						else if (comp.adjustableType == AdjustableComponent::Int) {
							ImGui::InputInt(comp.name.c_str(), &comp.i);
							if (comp.iPtr != nullptr)
								*comp.iPtr = comp.i;
						}
					}
				}
				ImGui::End();
			});
		}
	};
}