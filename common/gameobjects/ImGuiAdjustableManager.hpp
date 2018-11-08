#pragma once

#include "EntityManager.hpp"
#include "components/ImGuiComponent.hpp"
#include "components/AdjustableComponent.hpp"
#include "imgui.h"

namespace kengine {
	static auto ImGuiAdjustableManager(EntityManager & em) {
		return [&em](Entity & e) {
			e += ImGuiComponent([&em] {
				if (ImGui::Begin("Adjustables")) {
					static char nameSearch[1024] = "";
					ImGui::InputText("Name", nameSearch, sizeof(nameSearch));
					ImGui::Separator();

					std::vector<AdjustableComponent *> comps;

					for (const auto &[e, _] : em.getEntities<AdjustableComponent>()) {
						if (_.name.find(nameSearch) == std::string::npos)
							continue;
						comps.emplace_back(&_);
					}
					std::sort(comps.begin(), comps.end(), [](const auto lhs, const auto rhs) {
						return strcmp(lhs->name.c_str(), rhs->name.c_str()) < 0; 
					});

					for (const auto _ : comps) {
						ImGui::Columns(2);
						ImGui::Text(_->name.c_str());
						ImGui::NextColumn();

						switch (_->adjustableType) {
						case AdjustableComponent::String: {
							char buff[1024];
							const auto ret = _->s.copy(buff, sizeof(buff));
							buff[ret] = 0;

							ImGui::InputText(putils::concat("##", _->name).c_str(), buff, sizeof(buff));
							_->s = buff;
							if (_->sPtr != nullptr)
								*_->sPtr = _->s;
							break;
						}
						case AdjustableComponent::Bool: {
							ImGui::Checkbox(putils::concat("##", _->name).c_str(), &_->b);
							if (_->bPtr != nullptr)
								*_->bPtr = _->b;
							break;
						}
						case AdjustableComponent::Double: {
							ImGui::InputFloat(putils::concat("##", _->name).c_str(), &_->d);
							if (_->dPtr != nullptr)
								*_->dPtr = _->d;
							break;
						}
						case AdjustableComponent::Int: {
							ImGui::InputInt(putils::concat("##", _->name).c_str(), &_->i);
							if (_->iPtr != nullptr)
								*_->iPtr = _->i;
							break;
						}
						default:
							assert("Unknown type" && false);
						}
						ImGui::NextColumn();
					}
				}
				ImGui::End();
			});
		};
	}
}