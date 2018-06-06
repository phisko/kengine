#pragma once

#include "System.hpp"
#include "components/AdjustableComponent.hpp"
#include "components/ImGuiComponent.hpp"
#include "imgui.h"
#include "with.hpp"

namespace kengine {
	class ImGuiAdjustableSystem : public kengine::System<ImGuiAdjustableSystem> {
	public:
		ImGuiAdjustableSystem(kengine::EntityManager & em) : _em(em) {
			onLoad();
			_em.onLoad([this] { onLoad(); });
		}

	private:
		void onLoad() noexcept {
			_em.createOrAttach<kengine::ImGuiComponent>("imgui-adjustables", [this] {
				if (ImGui::Begin("Adjustables")) {
					static char nameSearch[1024] = "";
					ImGui::InputText("Name", nameSearch, sizeof(nameSearch));
					ImGui::Separator();

					auto objects = _em.getGameObjects<AdjustableComponent>();
					std::sort(objects.begin(), objects.end(), [](kengine::GameObject * first, kengine::GameObject * second) {
						return first->getComponent<AdjustableComponent>().name.compare(second->getComponent<AdjustableComponent>().name) < 0;
					});
					for (const auto go : objects) {

						{ pmeta_with(go->getComponent<AdjustableComponent>()) {
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
							}
							else if (_.adjustableType == AdjustableComponent::Bool) {
								ImGui::Checkbox(putils::concat("##", _.name).c_str(), &_.b);
								if (_.bPtr != nullptr)
									*_.bPtr = _.b;
							}
							else if (_.adjustableType == AdjustableComponent::Double) {
								ImGui::InputDouble(putils::concat("##", _.name).c_str(), &_.d);
								if (_.dPtr != nullptr)
									*_.dPtr = _.d;
							}
							else if (_.adjustableType == AdjustableComponent::Int) {
								ImGui::InputInt(putils::concat("##", _.name).c_str(), &_.i);
								if (_.iPtr != nullptr)
									*_.iPtr = _.i;
							}
							ImGui::NextColumn();
						}}
					}
				}
				ImGui::End();
			});
		}

		kengine::EntityManager & _em;
	};
}
