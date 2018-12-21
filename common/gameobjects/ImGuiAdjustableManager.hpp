#pragma once

#include <fstream>
#include "EntityManager.hpp"
#include "components/ImGuiComponent.hpp"
#include "components/AdjustableComponent.hpp"
#include "imgui.h"

namespace kengine {

	namespace detail {
		static constexpr auto saveFile = "adjust.cnf";
		static constexpr auto separator = ';';

		static void save(kengine::EntityManager & em) {
			std::ofstream f(saveFile, std::ofstream::trunc);

			for (const auto & [e, comp] : em.getEntities<AdjustableComponent>()) {
				f << comp.name << separator;
				switch (comp.adjustableType) {
				case AdjustableComponent::Int:
					f << comp.i;
					break;
				case AdjustableComponent::Double:
					f << comp.d;
					break;
				case AdjustableComponent::Bool:
					f << std::boolalpha << comp.b << std::noboolalpha;
					break;
				default:
					assert("Unknown adjustable type" && false);
				}
				f << '\n';
			}
		}

		static void load(kengine::EntityManager & em) {
			std::unordered_map<std::string, std::string> lines; {
				std::ifstream f(saveFile);
				for (std::string line; std::getline(f, line);) {
					const auto index = line.find(separator);
					lines[line.substr(0, index)] = line.substr(index + 1);
				}
			}

			for (auto & [e, comp] : em.getEntities<AdjustableComponent>()) {
				const auto it = lines.find(comp.name);
				if (it == lines.end())
					continue;

				const auto & val = it->second;

				const auto assignPtr = [](auto ptr, const auto & val) {
					if (ptr != nullptr)
						*ptr = val;
				};

				switch (comp.adjustableType) {
				case AdjustableComponent::Int:
					comp.i = putils::parse<int>(val);
					assignPtr(comp.iPtr, comp.i);
					break;
				case AdjustableComponent::Double:
					comp.d = putils::parse<float>(val);
					assignPtr(comp.dPtr, comp.d);
					break;
				case AdjustableComponent::Bool:
					comp.b = putils::parse<bool>(val);
					assignPtr(comp.bPtr, comp.b);
					break;
				default:
					assert("Unknown adjustable type" && false);
				}
			}
		}

		static std::pair<std::string, std::string> getNameAndSection(const std::string & fullName) {
			std::pair<std::string, std::string> ret;

			const auto sectionEnd = fullName.find(']');
			if (fullName[0] == '[' && sectionEnd != std::string::npos) {
				ret.first = fullName.substr(sectionEnd + 1);
				ret.second = fullName.substr(1, sectionEnd - 1);
			}
			else
				ret.first = fullName;

			return ret;
		}

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

		static size_t updateImGuiTree(bool & hidden, const std::vector<std::string> & subs, const std::vector<std::string> & previousSubsections) {
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

			return current;
		}

		static void draw(const std::string & name, AdjustableComponent & comp) {
			ImGui::Columns(2);
			ImGui::Text(name.c_str());
			ImGui::NextColumn();

			switch (comp.adjustableType) {
			case AdjustableComponent::Bool: {
				ImGui::Checkbox(putils::concat("##", comp.name).c_str(), &comp.b);
				if (comp.bPtr != nullptr)
					*comp.bPtr = comp.b;
				break;
			}
			case AdjustableComponent::Double: {
				ImGui::InputFloat(putils::concat("##", comp.name).c_str(), &comp.d);
				if (comp.dPtr != nullptr)
					*comp.dPtr = comp.d;
				break;
			}
			case AdjustableComponent::Int: {
				ImGui::InputInt(putils::concat("##", comp.name).c_str(), &comp.i);
				if (comp.iPtr != nullptr)
					*comp.iPtr = comp.i;
				break;
			}
			default:
				assert("Unknown type" && false);
			}
			ImGui::Columns();
		}
	}

	static auto ImGuiAdjustableManager(EntityManager & em) {
		return [&em](Entity & e) {
			e += ImGuiComponent([&em] {
				if (ImGui::Begin("Adjustables")) {
					static char nameSearch[1024] = "";
					ImGui::InputText("Name", nameSearch, sizeof(nameSearch));
					ImGui::Separator();

					ImGui::Columns(2);
					if (ImGui::Button("Save")) {
						detail::save(em);
					}
					ImGui::NextColumn();
					if (ImGui::Button("Load")) {
						detail::load(em);
					}
					ImGui::Columns();

					if (ImGui::BeginChild("##adjustables")) {
						std::vector<AdjustableComponent *> comps;

						for (const auto &[e, _] : em.getEntities<AdjustableComponent>()) {
							if (_.name.find(nameSearch) == (size_t)-1)
								continue;
							comps.emplace_back(&_);
						}
						std::sort(comps.begin(), comps.end(), [](const auto lhs, const auto rhs) {
							return strcmp(lhs->name.c_str(), rhs->name.c_str()) < 0;
						});

						std::vector<std::string> previousSubsections;
						std::string previousSection;
						bool hidden = false;
						for (const auto comp : comps) {
							const auto[name, section] = detail::getNameAndSection(comp->name);

							if (section != previousSection) {
								const auto subs = detail::split(section, '/');

								const auto current = detail::updateImGuiTree(hidden, subs, previousSubsections);

								previousSubsections = std::move(subs);
								if (current < previousSubsections.size())
									previousSubsections.erase(previousSubsections.begin() + current + 1, previousSubsections.end());
								previousSection = detail::reconstitutePath(previousSubsections);
							}

							if (hidden)
								continue;

							detail::draw(name, *comp);
						}
						for (size_t i = hidden ? 1 : 0; i < previousSubsections.size(); ++i)
							ImGui::TreePop();
					}
					ImGui::EndChild();
				}
				ImGui::End();
			});
		};
	}
}