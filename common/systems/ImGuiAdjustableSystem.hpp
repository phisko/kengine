#pragma once

#include "System.hpp"

#include <unordered_map>
#include <fstream>
#include "EntityManager.hpp"
#include "components/ImGuiComponent.hpp"
#include "components/AdjustableComponent.hpp"
#include "imgui.h"

#ifndef KENGINE_MAX_ADJUSTABLES_SECTIONS
# define KENGINE_MAX_ADJUSTABLES_SECTIONS 8
#endif

#ifndef KENGINE_MAX_ADJUSTABLES
# define KENGINE_MAX_ADJUSTABLES 256
#endif

namespace kengine {

	class ImGuiAdjustableSystem : public kengine::System<ImGuiAdjustableSystem, kengine::packets::RegisterEntity> {
		using string = AdjustableComponent::string;

	public:
		ImGuiAdjustableSystem(kengine::EntityManager & em);

		void onLoad() noexcept final {
			for (auto & [e, comp] : _em.getEntities<AdjustableComponent>()) {
				const auto it = _pointerSaves.find(comp.name);
				if (it == _pointerSaves.end()) {
					_em.removeEntity(e);
					continue;
				}

				comp.bPtr = it->second.bPtr;
				comp.iPtr = it->second.iPtr;
				comp.dPtr = it->second.dPtr;
			}

			load(_em);
		}

		void onSave() noexcept final {
			save(_em);
		}

		void handle(const packets::RegisterEntity & p) {
			if (!p.e.has<AdjustableComponent>())
				return;

			const auto & comp = p.e.get<AdjustableComponent>();
			auto & save = _pointerSaves[comp.name];
			save.bPtr = comp.bPtr;
			save.iPtr = comp.iPtr;
			save.dPtr = comp.dPtr;
		}

	private:
		kengine::EntityManager & _em;
		struct PointerSave {
			bool * bPtr;
			int * iPtr;
			float * dPtr;
		};
		std::unordered_map<string, PointerSave> _pointerSaves;

	private:
		static constexpr auto saveFile = "adjust.cnf";
		static constexpr auto separator = ';';

		static auto ImGuiAdjustableManager(EntityManager & em) {
			return [&em](Entity & e) {
				e += ImGuiComponent([&em] {
					if (ImGui::Begin("Adjustables")) {
						static char nameSearch[1024] = "";
						ImGui::InputText("Name", nameSearch, sizeof(nameSearch));
						ImGui::Separator();

						if (ImGui::BeginChild("##adjustables")) {
							putils::vector<AdjustableComponent *, KENGINE_MAX_ADJUSTABLES> comps;

							for (const auto &[e, _] : em.getEntities<AdjustableComponent>()) {
								if (_.name.find(nameSearch) == (size_t)-1)
									continue;
								comps.emplace_back(&_);
							}
							std::sort(comps.begin(), comps.end(), [](const auto lhs, const auto rhs) {
								return strcmp(lhs->name.c_str(), rhs->name.c_str()) < 0;
							});

							Sections previousSubsections;
							string previousSection;
							bool hidden = false;
							for (const auto comp : comps) {
								const auto[name, section] = getNameAndSection(comp->name);

								if (section != previousSection) {
									const auto subs = split(section, '/');

									const auto current = updateImGuiTree(hidden, subs, previousSubsections);

									previousSubsections = std::move(subs);
									if (current < previousSubsections.size())
										previousSubsections.erase(previousSubsections.begin() + current + 1, previousSubsections.end());
									previousSection = reconstitutePath(previousSubsections);
								}

								if (hidden)
									continue;

								draw(name.c_str(), *comp);
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

		static void save(kengine::EntityManager & em) {
			std::ofstream f(saveFile, std::ofstream::trunc);
			for (auto &[e, comp] : em.getEntities<AdjustableComponent>()) {
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

#ifndef NDEBUG
				comp.bPtr = nullptr;
				comp.iPtr = nullptr;
				comp.dPtr = nullptr;
#endif
			}
		}

		static void load(kengine::EntityManager & em) {
			em += ImGuiAdjustableManager(em);

			std::unordered_map<string, string> lines; {
				std::ifstream f(saveFile);
				for (std::string line; std::getline(f, line);) {
					const auto index = line.find(separator);
					lines[line.substr(0, index)] = line.substr(index + 1);
				}
			}

			for (auto &[e, comp] : em.getEntities<AdjustableComponent>()) {
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
					comp.i = putils::parse<int>(val.c_str());
					assignPtr(comp.iPtr, comp.i);
					break;
				case AdjustableComponent::Double:
					comp.d = putils::parse<float>(val.c_str());
					assignPtr(comp.dPtr, comp.d);
					break;
				case AdjustableComponent::Bool:
					comp.b = putils::parse<bool>(val.c_str());
					assignPtr(comp.bPtr, comp.b);
					break;
				default:
					assert("Unknown adjustable type" && false);
				}
			}
		}

		static std::pair<string, string> getNameAndSection(const string & fullName) {
			std::pair<string, string> ret;

			const auto sectionEnd = fullName.find(']');
			if (fullName.at(0) == '[' && sectionEnd != string::npos) {
				ret.first = fullName.substr(sectionEnd + 1);
				ret.second = fullName.substr(1, sectionEnd - 1);
			} else
				ret.first = fullName;

			return ret;
		}

		using Sections = putils::vector<string, KENGINE_MAX_ADJUSTABLES_SECTIONS>;
		static Sections split(const string & s, char delim) {
			Sections ret;

			size_t previous = 0;
			size_t next = 0;
			while (next < s.size()) {
				next = s.find(delim, previous);
				ret.emplace_back(s.substr(previous, next - previous));
				previous = next + 1;
			}

			return ret;
		}

		static string reconstitutePath(const Sections & subSections) {
			string ret;

			bool first = true;
			for (const auto & s : subSections) {
				if (!first)
					ret += '/';
				first = false;
				ret += s;
			}

			return ret;
		}

		static size_t updateImGuiTree(bool & hidden, const Sections & subs, const Sections & previousSubsections) {
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

		static void draw(const char * name, AdjustableComponent & comp) {
			ImGui::Columns(2);
			ImGui::Text(name);
			ImGui::NextColumn();

			switch (comp.adjustableType) {
			case AdjustableComponent::Bool: {
				ImGui::Checkbox((string("##") + comp.name).c_str(), &comp.b);
				if (comp.bPtr != nullptr)
					*comp.bPtr = comp.b;
				break;
			}
			case AdjustableComponent::Double: {
				ImGui::InputFloat((string("##") + comp.name).c_str(), &comp.d);
				if (comp.dPtr != nullptr)
					*comp.dPtr = comp.d;
				break;
			}
			case AdjustableComponent::Int: {
				ImGui::InputInt((string("##") + comp.name).c_str(), &comp.i);
				if (comp.iPtr != nullptr)
					*comp.iPtr = comp.i;
				break;
			}
			default:
				assert("Unknown type" && false);
			}
			ImGui::Columns();
		}
	};
	
	inline ImGuiAdjustableSystem::ImGuiAdjustableSystem(kengine::EntityManager & em) : System(em), _em(em) {
		em += ImGuiAdjustableManager(em);
	}
}