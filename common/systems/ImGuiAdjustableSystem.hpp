#pragma once

#include "System.hpp"
#include "components/AdjustableComponent.hpp"
#include "components/ImGuiComponent.hpp"
#include "imgui.h"
#include "with.hpp"

namespace kengine {
	class ImGuiAdjustableSystem : public kengine::System<ImGuiAdjustableSystem> {
	public:
		ImGuiAdjustableSystem(kengine::EntityManager & em) : System(em), _em(em) {
			_em.createEntity([this](kengine::Entity & e) {
				e.attach<kengine::ImGuiComponent>().setFunc([this] {
					if (ImGui::Begin("Adjustables")) {
						static char nameSearch[1024] = "";
						ImGui::InputText("Name", nameSearch, sizeof(nameSearch));
						ImGui::Separator();

						auto objects = _em.getEntities<AdjustableComponent>();
						for (const auto &[e, _] : objects) {
							if (_.name.find(nameSearch) == std::string::npos)
								continue;

							ImGui::Columns(2);
							ImGui::Text(_.name.c_str());
							ImGui::NextColumn();

							if (_.adjustableType == AdjustableComponent::String) {
								char buff[1024];
								_.s.copy(buff, sizeof(buff));
								ImGui::InputText(putils::concat("##", _.name).c_str(), buff, sizeof(buff));
								_.s = buff;
								if (_.sPtr != nullptr)
									*_.sPtr = _.s;
							} else if (_.adjustableType == AdjustableComponent::Bool) {
								ImGui::Checkbox(putils::concat("##", _.name).c_str(), &_.b);
								if (_.bPtr != nullptr)
									*_.bPtr = _.b;
							} else if (_.adjustableType == AdjustableComponent::Double) {
								ImGui::InputFloat(putils::concat("##", _.name).c_str(), &_.d);
								if (_.dPtr != nullptr)
									*_.dPtr = _.d;
							} else if (_.adjustableType == AdjustableComponent::Int) {
								ImGui::InputInt(putils::concat("##", _.name).c_str(), &_.i);
								if (_.iPtr != nullptr)
									*_.iPtr = _.i;
							}
							ImGui::NextColumn();
						}
					}
					ImGui::End();
				});
		});
	}

		kengine::EntityManager & _em;
	};
}
