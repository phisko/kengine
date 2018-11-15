#pragma once

#include "EntityManager.hpp"
#include "components/ImGuiComponent.hpp"
#include "components/AdjustableComponent.hpp"
#include "imgui.h"

namespace kengine {

	namespace detail {
		static std::vector<std::string> split(const std::string & s, char delim) {
			std::vector<std::string> ret;

			size_t previous = 0;
			size_t next = 0;
			while (next < s.size()) {
				next = s.find(delim, previous);
				ret.emplace_back(s.substr(previous, next - previous));
				previous = next + 1;
			}

			return ret;
		}

		static std::string reconstitutePath(const std::vector<std::string> & subSections) {
			std::string ret;

			bool first = true;
			for (const auto & s : subSections) {
				if (!first)
					ret += '/';
				first = false;
				ret += s;
			}

			return ret;
		}
	}

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

					std::vector<std::string> previousSubsections;
					std::string previousSection;
					bool hidden = false;
					for (const auto _ : comps) {
						std::string name;
						std::string section; {
							const auto sectionEnd = _->name.find(']');
							if (_->name[0] == '[' && sectionEnd != std::string::npos) {
								section = _->name.substr(1, sectionEnd - 1);
								name = _->name.substr(sectionEnd + 1);
							}
							else
								name = _->name;
						}

						if (section != previousSection) {
							const auto subs = detail::split(section, '/');

							auto current = previousSubsections.size() - 1;
							if (!previousSubsections.empty()) {
								while (current >= subs.size()) {
									if (!hidden)
										ImGui::TreePop();
									hidden = false;
									--current;
								}

								while (subs[current] != previousSubsections[current]) {
									if (current == 0) {
										if (!hidden)
											ImGui::TreePop();
										hidden = false;
										--current;
										break;
									}
									if (!hidden)
										ImGui::TreePop();
									hidden = false;
									--current;
								}
							}

							// if (hidden && current != std::numeric_limits<decltype(current)>::max() && subs[current] == previousSubsections[current])
							// 	continue;

							if (!hidden) {
								++current;
								while (current < subs.size()) {
									if (!ImGui::TreeNodeEx(subs[current].c_str())) {
										hidden = true;
										break;
									}
									++current;
								}
							}

							previousSubsections = std::move(subs);
							if (current < previousSubsections.size())
								previousSubsections.erase(previousSubsections.begin() + current + 1, previousSubsections.end());
							previousSection = detail::reconstitutePath(previousSubsections);
						}

						if (hidden)
							continue;

						ImGui::Columns(2);
						ImGui::Text(name.c_str());
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
						ImGui::Columns();
					}
					for (size_t i = hidden ? 1 : 0; i < previousSubsections.size(); ++i)
						ImGui::TreePop();
				}
				ImGui::End();
			});
		};
	}
}