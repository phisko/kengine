#include "ImGuiAdjustableSystem.hpp"
#include "EntityManager.hpp"

#include <fstream>

#include "data/AdjustableComponent.hpp"
#include "data/ImGuiToolComponent.hpp"
#include "data/NameComponent.hpp"

#include "functions/Execute.hpp"
#include "functions/OnTerminate.hpp"
#include "functions/OnEntityCreated.hpp"

#include "helpers/sortHelper.hpp"

#include "imgui.h"
#include "vector.hpp"
#include "to_string.hpp"
#include "magic_enum.hpp"
#include "regex.hpp"
#include "chop.hpp"
#include "visit.hpp"
#include "static_assert.hpp"

#ifndef KENGINE_DEFAULT_ADJUSTABLE_SAVE_PATH
# define KENGINE_DEFAULT_ADJUSTABLE_SAVE_PATH "."
#endif

#ifndef KENGINE_ADJUSTABLE_SAVE_FILE
# define KENGINE_ADJUSTABLE_SAVE_FILE "adjust.ini"
#endif

#ifndef KENGINE_ADJUSTABLE_SEPARATOR
# define KENGINE_ADJUSTABLE_SEPARATOR ';'
#endif

#ifndef KENGINE_MAX_ADJUSTABLES_SECTIONS
# define KENGINE_MAX_ADJUSTABLES_SECTIONS 8
#endif

#ifndef KENGINE_MAX_ADJUSTABLES
# define KENGINE_MAX_ADJUSTABLES 256
#endif

namespace kengine {
	using string = AdjustableComponent::string;

	using IniSection = std::unordered_map<string, string>;
	using IniFile = std::unordered_map<string, IniSection>;
	static IniFile g_loadedFile;

#pragma region declarations
	static void onEntityCreated(Entity & e);
	static void save(EntityManager & em);
	static void load(EntityManager & em);
	static void drawImGui(EntityManager & em, bool & enabled);
#pragma endregion
	EntityCreatorFunctor<64> ImGuiAdjustableSystem(EntityManager & em) {
		return [&](Entity & e) {
			load(em);
			e += functions::OnTerminate{ [&] { save(em); } };
			e += functions::OnEntityCreated{ onEntityCreated };

			e += NameComponent{ "Adjustables" };
			auto & tool = e.attach<ImGuiToolComponent>();
			tool.enabled = true;

			e += functions::Execute{[&](float deltaTime) {
				if (!tool.enabled)
					return;
				drawImGui(em, tool.enabled);
			}};
		};
	}

#pragma region drawImGui
#pragma region declarations
	static putils::vector<AdjustableComponent *, KENGINE_MAX_ADJUSTABLES> getFilteredComps(EntityManager & em, const char * nameSearch);
	using Sections = putils::vector<string, KENGINE_MAX_ADJUSTABLES_SECTIONS>;
	static Sections split(const string & s, char delim);
	static size_t updateImGuiTree(bool & hidden, const Sections & subs, const Sections & previousSubsections);
	static string reconstitutePath(const Sections & subSections);
	static void draw(const char * name, AdjustableComponent::Value & comp);
#pragma endregion
	static void drawImGui(EntityManager & em, bool & enabled) {
		if (ImGui::Begin("Adjustables", &enabled)) {
			static char nameSearch[1024] = "";

			ImGui::Columns(2);
			if (ImGui::Button("Save"))
				save(em);
			ImGui::NextColumn();
			if (ImGui::Button("Load"))
				load(em);
			ImGui::Columns();

			ImGui::Separator();
			ImGui::InputText("Name", nameSearch, sizeof(nameSearch));
			ImGui::Separator();

			if (ImGui::BeginChild("##adjustables")) {
				const auto comps = getFilteredComps(em, nameSearch);

				Sections previousSubsections;
				string previousSection;
				bool hidden = false;
				for (const auto comp : comps) {
					const bool sectionMatches = comp->section.find(nameSearch) != std::string::npos;

					if (comp->section != previousSection) {
						const auto subs = split(comp->section, '/');
						const auto current = updateImGuiTree(hidden, subs, previousSubsections);
						previousSubsections = subs;
						if (current < previousSubsections.size())
							previousSubsections.erase(previousSubsections.begin() + current + 1, previousSubsections.end());
						previousSection = reconstitutePath(previousSubsections);
					}

					if (hidden)
						continue;

					for (auto & value : comp->values)
						if (sectionMatches || value.name.find(nameSearch) != std::string::npos)
							draw(value.name.c_str(), value);
				}
				for (size_t i = hidden ? 1 : 0; i < previousSubsections.size(); ++i)
					ImGui::TreePop();
			}
			ImGui::EndChild();
		}
		ImGui::End();
	}

	static putils::vector<AdjustableComponent *, KENGINE_MAX_ADJUSTABLES> getFilteredComps(EntityManager & em, const char * nameSearch) {
		putils::vector<AdjustableComponent *, KENGINE_MAX_ADJUSTABLES> comps;

		for (const auto & [e, comp] : em.getEntities<AdjustableComponent>()) {
			if (comp.section.find(nameSearch) != std::string::npos) {
				comps.emplace_back(&comp);
				continue;
			}

			for (const auto & value : comp.values)
				if (value.name.find(nameSearch) != std::string::npos) {
					comps.emplace_back(&comp);
					continue;
				}
		}

		std::sort(comps.begin(), comps.end(), [](const auto lhs, const auto rhs) {
			return strcmp(lhs->section.c_str(), rhs->section.c_str()) < 0;
		});

		return comps;
	}

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

	static void draw(const char * name, AdjustableComponent::Value & value) {
		ImGui::Columns(2);
		ImGui::Text(name);
		ImGui::NextColumn();

		std::visit(putils::overloaded{
			[&](AdjustableComponent::Value::IntStorage & s) {
				if (value.getEnumNames != nullptr)
					ImGui::Combo((string("##") + value.name).c_str(), s.ptr != nullptr ? s.ptr : &s.value, value.getEnumNames(), (int)value.enumCount);
				else {
					ImGui::PushItemWidth(-1.f);
					auto val = s.ptr != nullptr ? *s.ptr : s.value;
					if (ImGui::InputInt((string("##") + value.name).c_str(), &val, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
						s.value = val;
						if (s.ptr != nullptr)
							*s.ptr = val;
					}
					ImGui::PopItemWidth();
				}
			},
			[&](AdjustableComponent::Value::FloatStorage & s) {
				ImGui::PushItemWidth(-1.f);
				auto val = s.ptr != nullptr ? *s.ptr : s.value;
				if (ImGui::InputFloat((string("##") + value.name).c_str(), &val, 0.f, 0.f, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue)) {
					s.value = val;
					if (s.ptr != nullptr)
						*s.ptr = val;
				}
				ImGui::PopItemWidth();
			},
			[&](AdjustableComponent::Value::BoolStorage & s) {
				ImGui::Checkbox((string("##") + value.name).c_str(), s.ptr != nullptr ? s.ptr : &s.value);
				if (s.ptr != nullptr)
					s.value = *s.ptr;
			},
			[&](AdjustableComponent::Value::ColorStorage & s) {
				const auto color = s.ptr != nullptr ? s.ptr->attributes : s.value.attributes;
				if (ImGui::ColorButton((string("##") + value.name).c_str(), ImVec4(color[0], color[1], color[2], color[3])))
					ImGui::OpenPopup("color picker popup");

				if (ImGui::BeginPopup("color picker popup")) {
					ImGui::ColorPicker4(value.name, color);
					ImGui::EndPopup();
				}

				if (s.ptr != nullptr)
					s.value = *s.ptr;
			},
			[&](auto && t) {
				static_assert(putils::always_false<decltype(t)>(), "Non exhaustive visitor");
			}
		}, value.storage);

		ImGui::Columns();
	}
#pragma endregion drawImGui

#pragma region onEntityCreated
#pragma region declarations
	static void initAdjustable(AdjustableComponent & comp);
#pragma endregion
	static void onEntityCreated(Entity & e) {
		if (!e.has<AdjustableComponent>())
			return;

		initAdjustable(e.get<AdjustableComponent>());
	}

#pragma region initAdjustable
#pragma region declarations
	static void setValue(AdjustableComponent::Value & value, const char * s);
#pragma endregion
	static void initAdjustable(AdjustableComponent & comp) {
		const auto it = g_loadedFile.find(comp.section);
		if (it == g_loadedFile.end())
			return;
		const auto & section = it->second;
		for (auto & value : comp.values) {
			const auto it = section.find(value.name);
			if (it != section.end())
				setValue(value, it->second);
		}
	}

	static void setValue(AdjustableComponent::Value & value, const char * s) {
		const auto assignPtr = [](auto & storage) {
			if (storage.ptr != nullptr)
				*storage.ptr = storage.value;
		};

		std::visit(putils::overloaded{
			[&](AdjustableComponent::Value::IntStorage & storage) {
				storage.value = putils::parse<int>(s);
				assignPtr(storage);
			},
			[&](AdjustableComponent::Value::FloatStorage & storage) {
				storage.value = putils::parse<float>(s);
				assignPtr(storage);
			},
			[&](AdjustableComponent::Value::BoolStorage & storage) {
				storage.value = putils::parse<bool>(s);
				assignPtr(storage);
			},
			[&](AdjustableComponent::Value::ColorStorage & storage) {
				putils::Color tmp;
				tmp.rgba = putils::parse<unsigned int>(s);
				storage.value = putils::toNormalizedColor(tmp);
				assignPtr(storage);
			},
			[&](auto && t) {
				static_assert(putils::always_false<decltype(t)>(), "Non exhaustive visitor");
			}
		}, value.storage);
	}
#pragma endregion initAdjustable

#pragma endregion onEntityCreated

	static void load(EntityManager & em) {
		std::ifstream f(KENGINE_ADJUSTABLE_SAVE_FILE);
		if (!f)
			return;
		IniSection * currentSection = nullptr;
		for (std::string line; std::getline(f, line);) {
			using namespace putils::regex;
			static const auto pattern = "^\\[(.*)\\]$"_m;
			const auto match = (line.c_str() == pattern);
			if (!match.empty()) {
				currentSection = &g_loadedFile[match[1].str()];
				continue;
			}
			else if (currentSection == nullptr) {
				assert(!"Invalid INI file, should start with a section");
				continue;
			}

			if (putils::chop(line).empty())
				continue;

			const auto index = line.find('=');
			if (index == std::string::npos) {
				assert(!"Invalid INI file, line should match 'key = value' format");
				continue;
			}

			const auto key = putils::chop(line.substr(0, index));
			const auto value = putils::chop(line.substr(index + 1));
			(*currentSection)[key] = value;
		}

		for (auto & [e, comp] : em.getEntities<AdjustableComponent>())
			initAdjustable(comp);
	}

	static void save(EntityManager & em) {
		std::ofstream f(KENGINE_ADJUSTABLE_SAVE_FILE, std::ofstream::trunc);
		assert(f);

		const auto entities = sortHelper::getSortedEntities<0, AdjustableComponent>(em, [](const auto & a, const auto & b) {
			return strcmp(std::get<1>(a)->section.c_str(), std::get<1>(b)->section.c_str()) < 0;
		});

		for (const auto & [e, comp] : entities) {
			f << '[' << comp->section << ']' << std::endl;

			auto values = comp->values;
			std::sort(values.begin(), values.end(), [](const auto & lhs, const auto & rhs) {
				return strcmp(lhs.name.c_str(), rhs.name.c_str()) < 0;
			});

			for (const auto & value : values) {
				f << value.name << '=';
				std::visit(putils::overloaded{
					[&](const AdjustableComponent::Value::IntStorage & s) {
						f << s.value;
					},
					[&](const AdjustableComponent::Value::FloatStorage & s) {
						f << s.value;
					},
					[&](const AdjustableComponent::Value::BoolStorage & s) {
						f << std::boolalpha << s.value << std::noboolalpha;
					},
					[&](const AdjustableComponent::Value::ColorStorage & s) {
						f << putils::toRGBA(s.value);
					},
					[&](auto && t) {
						static_assert(putils::always_false<decltype(t)>(), "Non exhaustive visitor");
					}
				}, value.storage);
				f << std::endl;
			}
			f << std::endl;
		}
	}

}
