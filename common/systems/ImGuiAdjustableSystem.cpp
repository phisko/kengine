#include "ImGuiAdjustableSystem.hpp"

#include <fstream>

#include "EntityManager.hpp"
#include "components/ImGuiComponent.hpp"
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

using string = kengine::AdjustableComponent::string;

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

static void draw(const char * name, kengine::AdjustableComponent & comp) {
	ImGui::Columns(2);
	ImGui::Text(name);
	ImGui::NextColumn();

	switch (comp.adjustableType) {
	case kengine::AdjustableComponent::Bool: {
		ImGui::Checkbox((string("##") + comp.name).c_str(), comp.bPtr != nullptr ? comp.bPtr : &comp.b);
		if (comp.bPtr != nullptr)
			comp.b = *comp.bPtr;
		break;
	}
	case kengine::AdjustableComponent::Double: {
		ImGui::PushItemWidth(-1.f);
		ImGui::InputFloat((string("##") + comp.name).c_str(), comp.dPtr != nullptr ? comp.dPtr : &comp.d, 0.f, 0.f, "%.6f");
		ImGui::PopItemWidth();
		if (comp.dPtr != nullptr)
			comp.d = *comp.dPtr;
		break;
	}
	case kengine::AdjustableComponent::Int: {
		ImGui::PushItemWidth(-1.f);
		ImGui::InputInt((string("##") + comp.name).c_str(), comp.iPtr != nullptr ? comp.iPtr : &comp.i);
		ImGui::PopItemWidth();
		if (comp.iPtr != nullptr)
			comp.i = *comp.iPtr;
		break;
	}
	case kengine::AdjustableComponent::Color: {
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
	case kengine::AdjustableComponent::Enum: {
		ImGui::Combo((string("##") + comp.name).c_str(), comp.iPtr != nullptr ? comp.iPtr : &comp.i, comp.getEnumNames(), (int)comp.enumCount);
		break;
	}
	default:
		assert("Unknown type" && false);
		static_assert(putils::magic_enum::enum_count<kengine::AdjustableComponent::EType>() == 5);
	}
	ImGui::Columns();
}

static void save(kengine::EntityManager & em, const char * directory) {
	std::ofstream f(putils::string<KENGINE_MAX_SAVE_PATH_LENGTH>("%s/%s", directory, KENGINE_ADJUSTABLE_SAVE_FILE), std::ofstream::trunc);
	assert(f);
	for (const auto &[e, comp] : em.getEntities<kengine::AdjustableComponent>()) {
		f << comp.name << KENGINE_ADJUSTABLE_SEPARATOR;
		switch (comp.adjustableType) {
		case kengine::AdjustableComponent::Int:
			f << comp.i;
			break;
		case kengine::AdjustableComponent::Double:
			f << comp.d;
			break;
		case kengine::AdjustableComponent::Bool:
			f << std::boolalpha << comp.b << std::noboolalpha;
			break;
		case kengine::AdjustableComponent::Color:
			f << putils::toRGBA(comp.color);
			break;
		case kengine::AdjustableComponent::Enum:
			f << comp.i;
			break;
		default:
			assert("Unknown adjustable type" && false);
			static_assert(putils::magic_enum::enum_count<kengine::AdjustableComponent::EType>() == 5);
		}
		f << '\n';
	}
}

static void setValue(kengine::AdjustableComponent & comp, const char * s) {
	const auto assignPtr = [](auto ptr, const auto & val) {
		if (ptr != nullptr)
			*ptr = val;
	};

	switch (comp.adjustableType) {
	case kengine::AdjustableComponent::Int:
		comp.i = putils::parse<int>(s);
		assignPtr(comp.iPtr, comp.i);
		break;
	case kengine::AdjustableComponent::Double:
		comp.d = putils::parse<float>(s);
		assignPtr(comp.dPtr, comp.d);
		break;
	case kengine::AdjustableComponent::Bool:
		comp.b = putils::parse<bool>(s);
		assignPtr(comp.bPtr, comp.b);
		break;
	case kengine::AdjustableComponent::Color: {
		putils::Color tmp;
		tmp.rgba = putils::parse<unsigned int>(s);
		comp.color = putils::toNormalizedColor(tmp);
		assignPtr(comp.colorPtr, comp.color);
		break;
	}
	case kengine::AdjustableComponent::Enum:
		comp.i = putils::parse<int>(s);
		assignPtr(comp.iPtr, comp.i);
		break;
	default:
		assert("Unknown adjustable type" && false);
		static_assert(putils::magic_enum::enum_count<kengine::AdjustableComponent::EType>() == 5);
	}
}

static std::unordered_map<string, string> g_loadedFile;
static void load(kengine::EntityManager & em, const char * directory) {
	std::ifstream f(putils::string<KENGINE_MAX_SAVE_PATH_LENGTH>("%s/%s", directory, KENGINE_ADJUSTABLE_SAVE_FILE));
	if (!f)
		return;
	for (std::string line; std::getline(f, line);) {
		const auto index = line.find(KENGINE_ADJUSTABLE_SEPARATOR);
		g_loadedFile[line.substr(0, index)] = line.substr(index + 1);
	}

	for (auto &[e, comp] : em.getEntities<kengine::AdjustableComponent>()) {
		const auto it = g_loadedFile.find(comp.name);
		if (it == g_loadedFile.end())
			continue;

		const auto & val = it->second;
		setValue(comp, val.c_str());
	}
}

static auto ImGuiAdjustableManager(kengine::EntityManager & em) {
	return [&em](kengine::Entity & e) {
		auto & tool = e.attach<kengine::ImGuiToolComponent>();
		tool.enabled = true;
		tool.name = "Adjustables";

		e += kengine::ImGuiComponent([&] {
			if (!tool.enabled)
				return;

			if (ImGui::Begin("Adjustables", &tool.enabled)) {
				static char nameSearch[1024] = "";

				ImGui::Columns(2);
				if (ImGui::Button("Save"))
					save(em, KENGINE_DEFAULT_ADJUSTABLE_SAVE_PATH);
				ImGui::NextColumn();
				if (ImGui::Button("Load"))
					load(em, KENGINE_DEFAULT_ADJUSTABLE_SAVE_PATH);
				ImGui::Columns();

				ImGui::Separator();
				ImGui::InputText("Name", nameSearch, sizeof(nameSearch));
				ImGui::Separator();

				if (ImGui::BeginChild("##adjustables")) {
					putils::vector<kengine::AdjustableComponent *, KENGINE_MAX_ADJUSTABLES> comps;

					for (const auto &[e, _] : em.getEntities<kengine::AdjustableComponent>()) {
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

kengine::ImGuiAdjustableSystem::ImGuiAdjustableSystem(kengine::EntityManager & em)
	: System(em), _em(em)
{
	for (auto &[e, comp] : _em.getEntities<AdjustableComponent>()) {
		auto & save = _pointerSaves[comp.name];
		save.bPtr = comp.bPtr;
		save.iPtr = comp.iPtr;
		save.dPtr = comp.dPtr;
	}

	onLoad(KENGINE_DEFAULT_ADJUSTABLE_SAVE_PATH);
}

void kengine::ImGuiAdjustableSystem::onLoad(const char * directory) noexcept {
	_em += ImGuiAdjustableManager(_em);

	for (auto &[e, comp] : _em.getEntities<AdjustableComponent>()) {
		const auto it = _pointerSaves.find(comp.name);
		if (it == _pointerSaves.end()) {
			_em.removeEntity(e);
			continue;
		}

		comp.bPtr = it->second.bPtr;
		comp.iPtr = it->second.iPtr;
		comp.dPtr = it->second.dPtr;
	}

	load(_em, directory);
}

void kengine::ImGuiAdjustableSystem::onSave(const char * directory) noexcept {
	save(_em, directory);
}

void kengine::ImGuiAdjustableSystem::handle(const packets::RegisterEntity & p) {
	if (!p.e.has<AdjustableComponent>())
		return;

	auto & comp = p.e.get<AdjustableComponent>();

	const auto it = g_loadedFile.find(comp.name);
	if (it != g_loadedFile.end())
		setValue(comp, it->second.c_str());

	auto & save = _pointerSaves[comp.name];
	save.bPtr = comp.bPtr;
	save.iPtr = comp.iPtr;
	save.dPtr = comp.dPtr;
}
