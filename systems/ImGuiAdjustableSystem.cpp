#include "ImGuiAdjustableSystem.hpp"
#include "EntityManager.hpp"

#include <fstream>

#include "data/AdjustableComponent.hpp"
#include "data/ImGuiComponent.hpp"
#include "data/ImGuiToolComponent.hpp"
#include "data/NameComponent.hpp"

#include "functions/OnTerminate.hpp"
#include "functions/OnEntityCreated.hpp"

#include "imgui.h"
#include "vector.hpp"
#include "to_string.hpp"
#include "magic_enum.hpp"
#include "regex.hpp"
#include "chop.hpp"

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

	// declarations
	static void onEntityCreated(Entity & e);
	static void save(EntityManager & em);
	static void load(EntityManager & em);
	//
	static std::pair<string, string> getNameAndSection(const string & fullName);
	using Sections = putils::vector<string, KENGINE_MAX_ADJUSTABLES_SECTIONS>;
	static Sections split(const string & s, char delim);
	static size_t updateImGuiTree(bool & hidden, const Sections & subs, const Sections & previousSubsections);
	static string reconstitutePath(const Sections & subSections);
	static void draw(const char * name, AdjustableComponent::Value & comp);
	//
	EntityCreatorFunctor<64> ImGuiAdjustableSystem(EntityManager & em) {
		return [&](Entity & e) {
			load(em);
			e += functions::OnTerminate{ [&] { save(em); } };
			e += functions::OnEntityCreated{ onEntityCreated };

			e += NameComponent{ "Adjustables" };
			auto & tool = e.attach<ImGuiToolComponent>();
			tool.enabled = true;

			e += ImGuiComponent([&] {
				if (!tool.enabled)
					return;

				if (ImGui::Begin("Adjustables", &tool.enabled)) {
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
			});
		};
	}

	// declarations
	static void initAdjustable(AdjustableComponent & comp);
	using IniSection = std::unordered_map<string, string>;
	using IniFile = std::unordered_map<string, IniSection>;
	static IniFile g_loadedFile;
	//
	static void onEntityCreated(Entity & e) {
		if (!e.has<AdjustableComponent>())
			return;

		initAdjustable(e.get<AdjustableComponent>());
	}

	static void load(EntityManager & em) {
		std::ifstream f(KENGINE_ADJUSTABLE_SAVE_FILE);
		if (!f)
			return;
		IniSection * currentSection = nullptr;
		for (std::string line; std::getline(f, line);) {
			using namespace putils::regex;
			const auto match = (line.c_str() == "^\\[(.*)\\]$"_m);
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

	// declarations
	static void setValue(AdjustableComponent::Value & value, const char * s);
	//
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

	static void save(EntityManager & em) {
		std::ofstream f(KENGINE_ADJUSTABLE_SAVE_FILE, std::ofstream::trunc);
		assert(f);
		for (const auto & [e, comp] : em.getEntities<AdjustableComponent>()) {
			f << '[' << comp.section << ']' << std::endl;
			for (const auto & value : comp.values) {
				f << value.name << '=';
				switch (value.adjustableType) {
				case AdjustableComponent::Value::Int:
					f << value.i.value;
					break;
				case AdjustableComponent::Value::Double:
					f << value.f.value;
					break;
				case AdjustableComponent::Value::Bool:
					f << std::boolalpha << value.b.value << std::noboolalpha;
					break;
				case AdjustableComponent::Value::Color:
					f << putils::toRGBA(value.color.value);
					break;
				case AdjustableComponent::Value::Enum:
					f << value.i.value;
					break;
				default:
					assert("Unknown adjustable type" && false);
					static_assert(putils::magic_enum::enum_count<AdjustableComponent::Value::EType>() == 5);
				}
				f << std::endl;
			}
			f << std::endl;
		}
	}

	static std::pair<string, string> getNameAndSection(const string & fullName) {
		std::pair<string, string> ret;

		const auto sectionEnd = fullName.find(']');
		if (fullName.at(0) == '[' && sectionEnd != string::npos) {
			ret.first = fullName.substr(sectionEnd + 1);
			ret.second = fullName.substr(1, sectionEnd - 1);
		}
		else
			ret.first = fullName;

		return ret;
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

		switch (value.adjustableType) {
		case AdjustableComponent::Value::Bool: {
			ImGui::Checkbox((string("##") + value.name).c_str(), value.b.ptr != nullptr ? value.b.ptr : &value.b.value);
			if (value.b.ptr != nullptr)
				value.b.value = *value.b.ptr;
			break;
		}
		case AdjustableComponent::Value::Double: {
			ImGui::PushItemWidth(-1.f);
			auto val = value.f.ptr != nullptr ? *value.f.ptr : value.f.value;
			if (ImGui::InputFloat((string("##") + value.name).c_str(), &val, 0.f, 0.f, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue)) {
				value.f.value = val;
				if (value.f.ptr != nullptr)
					*value.f.ptr = val;
			}
			ImGui::PopItemWidth();
			break;
		}
		case AdjustableComponent::Value::Int: {
			ImGui::PushItemWidth(-1.f);
			auto val = value.i.ptr != nullptr ? *value.i.ptr : value.i.value;
			if (ImGui::InputInt((string("##") + value.name).c_str(), &val, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
				value.i.value = val;
				if (value.i.ptr != nullptr)
					*value.i.ptr = val;
			}
			ImGui::PopItemWidth();
			break;
		}
		case AdjustableComponent::Value::Color: {
			const auto color = value.color.ptr != nullptr ? value.color.ptr->attributes : value.color.value.attributes;
			if (ImGui::ColorButton((string("##") + value.name).c_str(), ImVec4(color[0], color[1], color[2], color[3])))
				ImGui::OpenPopup("color picker popup");

			if (ImGui::BeginPopup("color picker popup")) {
				ImGui::ColorPicker4(value.name, color);
				ImGui::EndPopup();
			}

			if (value.color.ptr != nullptr)
				value.color.value = *value.color.ptr;
			break;
		}
		case AdjustableComponent::Value::Enum: {
			ImGui::Combo((string("##") + value.name).c_str(), value.i.ptr != nullptr ? value.i.ptr : &value.i.value, value.getEnumNames(), (int)value.enumCount);
			break;
		}
		default:
			assert("Unknown type" && false);
			static_assert(putils::magic_enum::enum_count<AdjustableComponent::Value::EType>() == 5);
		}
		ImGui::Columns();
	}

	static void setValue(AdjustableComponent::Value & value, const char * s) {
		const auto assignPtr = [](auto & storage) {
			if (storage.ptr != nullptr)
				*storage.ptr = storage.value;
		};

		switch (value.adjustableType) {
		case AdjustableComponent::Value::Int:
			value.i.value = putils::parse<int>(s);
			assignPtr(value.i);
			break;
		case AdjustableComponent::Value::Double:
			value.f.value = putils::parse<float>(s);
			assignPtr(value.f);
			break;
		case AdjustableComponent::Value::Bool:
			value.b.value = putils::parse<bool>(s);
			assignPtr(value.b);
			break;
		case AdjustableComponent::Value::Color: {
			putils::Color tmp;
			tmp.rgba = putils::parse<unsigned int>(s);
			value.color.value = putils::toNormalizedColor(tmp);
			assignPtr(value.color);
			break;
		}
		case AdjustableComponent::Value::Enum:
			value.i.value = putils::parse<int>(s);
			assignPtr(value.i);
			break;
		default:
			assert("Unknown adjustable type" && false);
			static_assert(putils::magic_enum::enum_count<AdjustableComponent::Value::EType>() == 5);
		}
	}
}
