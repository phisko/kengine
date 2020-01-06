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

#ifndef KENGINE_DEFAULT_ADJUSTABLE_SAVE_PATH
# define KENGINE_DEFAULT_ADJUSTABLE_SAVE_PATH "."
#endif

#ifndef KENGINE_ADJUSTABLE_SAVE_FILE
# define KENGINE_ADJUSTABLE_SAVE_FILE "adjust.cnf"
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
	static void draw(const char * name, AdjustableComponent & comp);
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

						for (const auto & [e, _] : em.getEntities<AdjustableComponent>()) {
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
							const auto [name, section] = getNameAndSection(comp->name);

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

	// declarations
	static void setValue(AdjustableComponent & comp, const char * s);
	static std::unordered_map<string, string> g_loadedFile;
	//
	static void onEntityCreated(Entity & e) {
		if (!e.has<AdjustableComponent>())
			return;

		auto & comp = e.get<AdjustableComponent>();

		const auto it = g_loadedFile.find(comp.name);
		if (it != g_loadedFile.end())
			setValue(comp, it->second.c_str());
	}

	static void load(EntityManager & em) {
		std::ifstream f(KENGINE_ADJUSTABLE_SAVE_FILE);
		if (!f)
			return;
		for (std::string line; std::getline(f, line);) {
			const auto index = line.find(KENGINE_ADJUSTABLE_SEPARATOR);
			g_loadedFile[line.substr(0, index)] = line.substr(index + 1);
		}

		for (auto & [e, comp] : em.getEntities<AdjustableComponent>()) {
			const auto it = g_loadedFile.find(comp.name);
			if (it == g_loadedFile.end())
				continue;

			const auto & val = it->second;
			setValue(comp, val.c_str());
		}
	}

	static void save(EntityManager & em) {
		std::ofstream f(KENGINE_ADJUSTABLE_SAVE_FILE, std::ofstream::trunc);
		assert(f);
		for (const auto & [e, comp] : em.getEntities<AdjustableComponent>()) {
			f << comp.name << KENGINE_ADJUSTABLE_SEPARATOR;
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
			case AdjustableComponent::Color:
				f << putils::toRGBA(comp.color);
				break;
			case AdjustableComponent::Enum:
				f << comp.i;
				break;
			default:
				assert("Unknown adjustable type" && false);
				static_assert(putils::magic_enum::enum_count<AdjustableComponent::EType>() == 5);
			}
			f << '\n';
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

	static void draw(const char * name, AdjustableComponent & comp) {
		ImGui::Columns(2);
		ImGui::Text(name);
		ImGui::NextColumn();

		switch (comp.adjustableType) {
		case AdjustableComponent::Bool: {
			ImGui::Checkbox((string("##") + comp.name).c_str(), comp.bPtr != nullptr ? comp.bPtr : &comp.b);
			if (comp.bPtr != nullptr)
				comp.b = *comp.bPtr;
			break;
		}
		case AdjustableComponent::Double: {
			ImGui::PushItemWidth(-1.f);
			auto val = comp.dPtr != nullptr ? *comp.dPtr : comp.d;
			if (ImGui::InputFloat((string("##") + comp.name).c_str(), &val, 0.f, 0.f, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue)) {
				comp.d = val;
				if (comp.dPtr != nullptr)
					*comp.dPtr = val;
			}
			ImGui::PopItemWidth();
			break;
		}
		case AdjustableComponent::Int: {
			ImGui::PushItemWidth(-1.f);
			auto val = comp.iPtr != nullptr ? *comp.iPtr : comp.i;
			if (ImGui::InputInt((string("##") + comp.name).c_str(), comp.iPtr != nullptr ? comp.iPtr : &comp.i, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
				comp.i = val;
				if (comp.iPtr != nullptr)
					*comp.iPtr = val;
			}
			ImGui::PopItemWidth();
			break;
		}
		case AdjustableComponent::Color: {
			const auto color = comp.colorPtr != nullptr ? comp.colorPtr->attributes : comp.color.attributes;
			if (ImGui::ColorButton((string("##") + comp.name).c_str(), ImVec4(color[0], color[1], color[2], color[3])))
				ImGui::OpenPopup("color picker popup");

			if (ImGui::BeginPopup("color picker popup")) {
				ImGui::ColorPicker4(comp.name, color);
				ImGui::EndPopup();
			}

			if (comp.colorPtr != nullptr)
				comp.color = *comp.colorPtr;
			break;
		}
		case AdjustableComponent::Enum: {
			ImGui::Combo((string("##") + comp.name).c_str(), comp.iPtr != nullptr ? comp.iPtr : &comp.i, comp.getEnumNames(), (int)comp.enumCount);
			break;
		}
		default:
			assert("Unknown type" && false);
			static_assert(putils::magic_enum::enum_count<AdjustableComponent::EType>() == 5);
		}
		ImGui::Columns();
	}

	static void setValue(AdjustableComponent & comp, const char * s) {
		const auto assignPtr = [](auto ptr, const auto & val) {
			if (ptr != nullptr)
				*ptr = val;
		};

		switch (comp.adjustableType) {
		case AdjustableComponent::Int:
			comp.i = putils::parse<int>(s);
			assignPtr(comp.iPtr, comp.i);
			break;
		case AdjustableComponent::Double:
			comp.d = putils::parse<float>(s);
			assignPtr(comp.dPtr, comp.d);
			break;
		case AdjustableComponent::Bool:
			comp.b = putils::parse<bool>(s);
			assignPtr(comp.bPtr, comp.b);
			break;
		case AdjustableComponent::Color: {
			putils::Color tmp;
			tmp.rgba = putils::parse<unsigned int>(s);
			comp.color = putils::toNormalizedColor(tmp);
			assignPtr(comp.colorPtr, comp.color);
			break;
		}
		case AdjustableComponent::Enum:
			comp.i = putils::parse<int>(s);
			assignPtr(comp.iPtr, comp.i);
			break;
		default:
			assert("Unknown adjustable type" && false);
			static_assert(putils::magic_enum::enum_count<AdjustableComponent::EType>() == 5);
		}
	}
}
