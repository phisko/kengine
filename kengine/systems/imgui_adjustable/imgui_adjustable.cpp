#include "imgui_adjustable.hpp"

// stl
#include <fstream>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// magic_enum
#include <magic_enum.hpp>

// imgui
#include <imgui.h>

// putils
#include "putils/forward_to.hpp"
#include "putils/vector.hpp"
#include "putils/to_string.hpp"
#include "putils/visit.hpp"
#include "putils/ini_file.hpp"
#include "putils/static_assert.hpp"

// kengine data
#include "kengine/data/adjustable.hpp"
#include "kengine/data/imgui_tool.hpp"
#include "kengine/data/name.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/assert_helper.hpp"
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

#ifndef KENGINE_DEFAULT_ADJUSTABLE_SAVE_PATH
#define KENGINE_DEFAULT_ADJUSTABLE_SAVE_PATH "."
#endif

#ifndef KENGINE_ADJUSTABLE_SAVE_FILE
#define KENGINE_ADJUSTABLE_SAVE_FILE "adjust.ini"
#endif

#ifndef KENGINE_ADJUSTABLE_SEPARATOR
#define KENGINE_ADJUSTABLE_SEPARATOR ';'
#endif

#ifndef KENGINE_MAX_ADJUSTABLES_SECTIONS
#define KENGINE_MAX_ADJUSTABLES_SECTIONS 8
#endif

#ifndef KENGINE_MAX_ADJUSTABLES
#define KENGINE_MAX_ADJUSTABLES 256
#endif

namespace kengine::systems {
	struct imgui_adjustable {
		entt::registry & r;
		entt::scoped_connection connection;

		bool * enabled;
		putils::ini_file loaded_file;

		imgui_adjustable(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;

			load();

			e.emplace<functions::execute>(putils_forward_to_this(execute));

			e.emplace<data::name>("Adjustables");
			auto & tool = e.emplace<data::imgui_tool>();
			tool.enabled = true;
			enabled = &tool.enabled;

			connection = r.on_construct<data::adjustable>().connect<&imgui_adjustable::init_adjustable>(this);
		}

		~imgui_adjustable() noexcept {
			KENGINE_PROFILING_SCOPE;
			save();
		}

		char name_search[1024] = "";
		using string = data::adjustable::string;
		using sections = putils::vector<string, KENGINE_MAX_ADJUSTABLES_SECTIONS>;
		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!*enabled)
				return;
			kengine_log(r, verbose, "execute", "imgui_adjustable");

			if (ImGui::Begin("Adjustables", enabled)) {
				ImGui::Columns(2);
				if (ImGui::Button("Save", { -1.f, 0.f }))
					save();
				ImGui::NextColumn();
				if (ImGui::Button("Load", { -1.f, 0.f }))
					load();
				ImGui::Columns();

				ImGui::Separator();
				ImGui::InputText("Name", name_search, sizeof(name_search));
				ImGui::Separator();

				if (ImGui::BeginChild("##adjustables")) {
					const auto comps = get_filtered_comps(name_search);

					sections previous_subsections;
					string previous_section;
					bool hidden = false;
					for (const auto comp : comps) {
						const bool section_matches = comp->section.find(name_search) != std::string::npos;

						if (comp->section != previous_section) {
							const auto subs = split(comp->section, '/');
							const auto current = update_imgui_tree(hidden, subs, previous_subsections);
							previous_subsections = subs;
							if (current < previous_subsections.size())
								previous_subsections.erase(previous_subsections.begin() + current + 1, previous_subsections.end());
							previous_section = reconstitute_path(previous_subsections);
						}

						if (hidden)
							continue;

						for (auto & value : comp->values)
							if (section_matches || value.name.find(name_search) != std::string::npos)
								draw(value.name.c_str(), value);
					}
					for (size_t i = hidden ? 1 : 0; i < previous_subsections.size(); ++i)
						ImGui::TreePop();
				}
				ImGui::EndChild();
			}
			ImGui::End();
		}

		putils::vector<data::adjustable *, KENGINE_MAX_ADJUSTABLES> get_filtered_comps(const char * name_search) noexcept {
			KENGINE_PROFILING_SCOPE;

			putils::vector<data::adjustable *, KENGINE_MAX_ADJUSTABLES> comps;

			for (const auto & [e, comp] : r.view<data::adjustable>().each()) {
				if (comp.section.find(name_search) != std::string::npos) {
					comps.emplace_back(&comp);
					continue;
				}

				for (const auto & value : comp.values)
					if (value.name.find(name_search) != std::string::npos) {
						comps.emplace_back(&comp);
						continue;
					}
			}

			std::sort(comps.begin(), comps.end(), [](const auto lhs, const auto rhs) noexcept {
				return strcmp(lhs->section.c_str(), rhs->section.c_str()) < 0;
			});

			return comps;
		}

		sections split(const string & s, char delim) noexcept {
			KENGINE_PROFILING_SCOPE;

			sections ret;

			size_t previous = 0;
			size_t next = 0;
			while (next < s.size()) {
				next = s.find(delim, previous);
				if (next == string::npos)
					next = s.size();
				ret.emplace_back(s.substr(previous, next - previous));
				previous = next + 1;
			}

			return ret;
		}

		string reconstitute_path(const sections & sub_sections) noexcept {
			KENGINE_PROFILING_SCOPE;

			string ret;

			bool first = true;
			for (const auto & s : sub_sections) {
				if (!first)
					ret += '/';
				first = false;
				ret += s;
			}

			return ret;
		}

		size_t update_imgui_tree(bool & hidden, const sections & subs, const sections & previous_subsections) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto current = previous_subsections.size() - 1;

			if (!previous_subsections.empty()) {
				while (current >= subs.size()) {
					if (!hidden)
						ImGui::TreePop();
					hidden = false;
					--current;
				}

				while (subs[current] != previous_subsections[current]) {
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

		void draw(const char * name, data::adjustable::value & value) noexcept {
			KENGINE_PROFILING_SCOPE;

			ImGui::Columns(2);
			ImGui::Text("%s", name);
			ImGui::NextColumn();

			switch (value.type) {
				case data::adjustable::value_type::Int: {
					auto & s = value.int_storage;
					if (value.get_enum_names != nullptr)
						ImGui::Combo((string("##") + value.name).c_str(), s.ptr != nullptr ? s.ptr : &s.value, value.get_enum_names(), (int)value.enum_count);
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
					break;
				}
				case data::adjustable::value_type::Float: {
					auto & s = value.float_storage;
					ImGui::PushItemWidth(-1.f);
					auto val = s.ptr != nullptr ? *s.ptr : s.value;
					if (ImGui::InputFloat((string("##") + value.name).c_str(), &val, 0.f, 0.f, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue)) {
						s.value = val;
						if (s.ptr != nullptr)
							*s.ptr = val;
					}
					ImGui::PopItemWidth();
					break;
				}
				case data::adjustable::value_type::Bool: {
					auto & s = value.bool_storage;
					ImGui::Checkbox((string("##") + value.name).c_str(), s.ptr != nullptr ? s.ptr : &s.value);
					if (s.ptr != nullptr)
						s.value = *s.ptr;
					break;
				}
				case data::adjustable::value_type::Color: {
					auto & s = value.color_storage;
					const auto color = s.ptr != nullptr ? s.ptr->attributes : s.value.attributes;
					if (ImGui::ColorButton((string("##") + value.name).c_str(), ImVec4(color[0], color[1], color[2], color[3])))
						ImGui::OpenPopup("color picker popup");

					if (ImGui::BeginPopup("color picker popup")) {
						ImGui::ColorPicker4(value.name.c_str(), color);
						ImGui::EndPopup();
					}
					if (s.ptr != nullptr)
						s.value = *s.ptr;
					break;
				}
				default: {
					static_assert(magic_enum::enum_count<data::adjustable::value_type>() == 5); // + 1 for Invalid
					kengine_assert_failed(r, "Unknown data::adjustable::value type");
					break;
				}
			}

			ImGui::Columns();
		}

		void init_adjustable(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & comp = r.get<data::adjustable>(e);
			kengine_logf(r, log, "Init/systems/imgui_adjustable", "Initializing section %s", comp.section.c_str());

			const auto it = loaded_file.sections.find(comp.section.c_str());
			if (it == loaded_file.sections.end()) {
				kengine_logf(r, warning, "Init/systems/imgui_adjustable", "Section '%s' not found in INI file", comp.section.c_str());
				return;
			}
			const auto & section = it->second;
			for (auto & value : comp.values) {
				const auto it = section.values.find(value.name.c_str());
				if (it != section.values.end()) {
					kengine_logf(r, log, "Init/systems/imgui_adjustable", "Initializing %s", value.name.c_str());
					set_value(value, it->second.c_str());
				}
				else
					kengine_logf(r, log, "Init/systems/imgui_adjustable", "value not found in INI for %s", value.name.c_str());
			}
		}

		void set_value(data::adjustable::value & value, const char * s) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto assign_ptr = [](auto & storage) {
				if (storage.ptr != nullptr)
					*storage.ptr = storage.value;
			};

			switch (value.type) {
				case data::adjustable::value_type::Int: {
					value.int_storage.value = putils::parse<int>(s);
					assign_ptr(value.int_storage);
					break;
				}
				case data::adjustable::value_type::Float: {
					value.float_storage.value = putils::parse<float>(s);
					assign_ptr(value.float_storage);
					break;
				}
				case data::adjustable::value_type::Bool: {
					value.bool_storage.value = putils::parse<bool>(s);
					assign_ptr(value.bool_storage);
					break;
				}
				case data::adjustable::value_type::Color: {
					putils::color tmp;
					tmp.rgba = putils::parse<unsigned int>(s);
					value.color_storage.value = putils::to_normalized_color(tmp);
					assign_ptr(value.color_storage);
					break;
				}
				default: {
					static_assert(magic_enum::enum_count<data::adjustable::value_type>() == 5); // + 1 for Invalid
					kengine_assert_failed(r, "Unknown data::adjustable::value type");
					break;
				}
			}
		}

		void load() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "systems/imgui_adjustable", "Loading from " KENGINE_ADJUSTABLE_SAVE_FILE);

			std::ifstream f(KENGINE_ADJUSTABLE_SAVE_FILE);
			f >> loaded_file;
			for (auto [e, comp] : r.view<data::adjustable>().each())
				init_adjustable(r, e);
		}

		void save() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "systems/imgui_adjustable", "Saving to " KENGINE_ADJUSTABLE_SAVE_FILE);

			std::ofstream f(KENGINE_ADJUSTABLE_SAVE_FILE, std::ofstream::trunc);
			if (!f) {
				kengine_assert_failed(r, "Failed to open '", KENGINE_ADJUSTABLE_SAVE_FILE, "' with write permissions");
				return;
			}

			putils::ini_file ini;

			for (const auto & [e, comp] : r.view<data::adjustable>().each()) {
				auto & section = ini.sections[comp.section.c_str()];

				for (const auto & value : comp.values) {
					auto & ini_value = section.values[value.name.c_str()];

					switch (value.type) {
						case data::adjustable::value_type::Int: {
							ini_value = putils::to_string(value.int_storage.value);
							break;
						}
						case data::adjustable::value_type::Float: {
							ini_value = putils::to_string(value.float_storage.value);
							break;
						}
						case data::adjustable::value_type::Bool: {
							ini_value = putils::to_string(value.bool_storage.value);
							break;
						}
						case data::adjustable::value_type::Color: {
							ini_value = putils::to_string(putils::to_rgba(value.color_storage.value));
							break;
						}
						default: {
							static_assert(magic_enum::enum_count<data::adjustable::value_type>() == 5); // + 1 for Invalid
							kengine_assert_failed(r, "Unknown data::adjustable::value type");
							break;
						}
					}
				}
			}

			f << ini;
		}
	};

	void add_imgui_adjustable(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<imgui_adjustable>(e);
	}
}
