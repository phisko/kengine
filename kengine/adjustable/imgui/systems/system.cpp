#include "system.hpp"

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
#include "putils/ini_file.hpp"
#include "putils/scn/scn.hpp"
#include "putils/split.hpp"
#include "putils/static_assert.hpp"
#include "putils/vector.hpp"

// kengine core
#include "kengine/core/data/name.hpp"
#include "kengine/core/helpers/new_entity_processor.hpp"

// kengine core/assert
#include "kengine/core/assert/helpers/kengine_assert.hpp"

// kengine core/log
#include "kengine/core/log/helpers/kengine_log.hpp"

// kengine core/profiling
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine adjustable
#include "kengine/adjustable/data/adjustable.hpp"

// kengine imgui
#include "kengine/imgui/helpers/set_context.hpp"

// kengine imgui/imgui_tool
#include "kengine/imgui/tool/data/tool.hpp"

// kengine main_loop
#include "kengine/main_loop/functions/execute.hpp"

#ifndef KENGINE_DEFAULT_ADJUSTABLE_SAVE_PATH
#define KENGINE_DEFAULT_ADJUSTABLE_SAVE_PATH "."
#endif

#ifndef KENGINE_ADJUSTABLE_SAVE_FILE
#define KENGINE_ADJUSTABLE_SAVE_FILE "adjust.ini"
#endif

#ifndef KENGINE_ADJUSTABLE_SEPARATOR
#define KENGINE_ADJUSTABLE_SEPARATOR ';'
#endif

#ifndef KENGINE_MAX_ADJUSTABLES
#define KENGINE_MAX_ADJUSTABLES 256
#endif

namespace kengine::adjustable::imgui {
	static constexpr auto log_category = "adjustable_imgui";

	struct system {
		entt::registry & r;
		const entt::scoped_connection connection = r.on_destroy<adjustable>().connect<&system::on_destroy_adjustable>(this);

		bool * enabled;
		putils::ini_file loaded_file = load_ini_file();

		struct section {
			using section_map = std::map<std::string, section>;
			section_map subsections;

			struct entry {
				adjustable * component = nullptr;
				std::vector<bool> values_pass_search; // Indexed by adjustable->values
			};

			std::vector<entry> entries;
			bool passes_search = true;
		};
		section root_section;

		struct processed {};
		kengine::new_entity_processor<processed, adjustable> processor{ r, putils_forward_to_this(on_construct_adjustable) };

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));

			e.emplace<core::name>("Adjustables");
			auto & tool = e.emplace<kengine::imgui::tool::tool>();
			enabled = &tool.enabled;

			processor.process();
		}

		char name_search[1024] = "";
		using string = adjustable::string;
		using sections = std::vector<std::string>;
		bool search_out_of_date = true;
		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");

			if (!*enabled) {
				kengine_log(r, very_verbose, log_category, "Disabled");
				return;
			}

			if (!kengine::imgui::set_context(r))
				return;

			processor.process();

			if (ImGui::Begin("Adjustables", enabled)) {
				ImGui::Columns(2);
				if (ImGui::Button("Save", { -1.f, 0.f }))
					save();
				ImGui::NextColumn();
				if (ImGui::Button("Load", { -1.f, 0.f }))
					loaded_file = load_ini_file();
				ImGui::Columns();

				ImGui::Separator();
				if (ImGui::InputText("Name", name_search, sizeof(name_search))) {
					kengine_logf(r, verbose, log_category, "Name search changed to '{}'", name_search);
					search_out_of_date = true;
				}
				ImGui::Separator();

				if (search_out_of_date) {
					kengine_log(r, verbose, log_category, "Updating search results");
					update_search_results("", root_section);
					search_out_of_date = false;
				}

				if (ImGui::BeginChild("##adjustables"))
					for (const auto & [name, section] : root_section.subsections)
						display_menu_entry(name.c_str(), section);
				ImGui::EndChild();
			}
			ImGui::End();
		}

		void update_search_results(const std::string & name, section & section) noexcept {
			KENGINE_PROFILING_SCOPE;

			section.passes_search = false;

			for (auto & [subentry_name, subsection] : section.subsections) {
				update_search_results(name + '/' + subentry_name, subsection);
				if (subsection.passes_search)
					section.passes_search = true;
			}

			for (auto & entry : section.entries) {
				entry.values_pass_search.clear();
				entry.values_pass_search.resize(entry.component->values.size());

				size_t index = 0;
				for (const auto & value : entry.component->values) {
					const auto value_name = name + '/' + value.name.c_str();

					if (value_name.find(name_search) != std::string::npos) {
						entry.values_pass_search[index] = true;
						section.passes_search = true;
					}

					++index;
				}
			}
		}

		void display_menu_entry(const char * name, const section & section) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!section.passes_search)
				return;

			if (ImGui::TreeNodeEx(name)) {
				for (const auto & entry : section.entries) {
					size_t i = 0;
					for (auto & value : entry.component->values) {
						if (entry.values_pass_search[i])
							draw(value);
						++i;
					}
				}

				for (const auto & [subsection_name, subsection] : section.subsections)
					display_menu_entry(subsection_name.c_str(), subsection);

				ImGui::TreePop();
			}
		}

		void draw(adjustable::value & value) noexcept {
			KENGINE_PROFILING_SCOPE;

			ImGui::Columns(2);
			ImGui::Text("%s", value.name.c_str());
			ImGui::NextColumn();

			switch (value.type) {
				case adjustable::value_type::Int: {
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
				case adjustable::value_type::Float: {
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
				case adjustable::value_type::Bool: {
					auto & s = value.bool_storage;
					ImGui::Checkbox((string("##") + value.name).c_str(), s.ptr != nullptr ? s.ptr : &s.value);
					if (s.ptr != nullptr)
						s.value = *s.ptr;
					break;
				}
				case adjustable::value_type::Color: {
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
					static_assert(magic_enum::enum_count<adjustable::value_type>() == 5); // + 1 for Invalid
					kengine_assert_failed(r, "Unknown adjustable::value type");
					break;
				}
			}

			ImGui::Columns();
		}

		void on_construct_adjustable(entt::entity e, adjustable & comp) noexcept {
			KENGINE_PROFILING_SCOPE;

			init_adjustable(comp);

			section * current_section = &root_section;
			const auto section_names = putils::split(comp.section.c_str(), '/');
			for (const auto & section_name : section_names)
				current_section = &current_section->subsections[section_name];

			section::entry entry;
			entry.component = &comp;
			current_section->entries.push_back(std::move(entry));

			search_out_of_date = true;
		}

		void on_destroy_adjustable(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & comp = r.get<adjustable>(e);
			kengine_logf(r, verbose, log_category, "Adjustable destroyed in {} (section {})", e, comp.section);
			remove_adjustable_from_section(comp, root_section);
		}

		bool remove_adjustable_from_section(const adjustable & comp, section & section) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (const auto it = std::ranges::find_if(section.entries, [&](const auto & entry) { return entry.component == &comp; }); it != section.entries.end()) {
				section.entries.erase(it);
				return true;
			}

			for (auto it = section.subsections.begin(); it != section.subsections.end(); ++it) {
				auto & subsection = it->second;
				if (remove_adjustable_from_section(comp, subsection)) {
					if (subsection.subsections.empty())
						section.subsections.erase(it);
					return true;
				}
			}

			search_out_of_date = true;
			return false;
		}

		void init_adjustable(adjustable & comp) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(r, verbose, log_category, "Initializing section {}", comp.section);

			const auto it = loaded_file.sections.find(comp.section.c_str());
			if (it == loaded_file.sections.end()) {
				kengine_logf(r, warning, log_category, "Section '{}' not found in INI file", comp.section);
				return;
			}

			const auto & section = it->second;
			for (auto & value : comp.values) {
				const auto it = section.values.find(value.name.c_str());
				if (it == section.values.end()) {
					kengine_logf(r, warning, log_category, "Value for '{}' not found in INI file", value.name);
					continue;
				}

				kengine_logf(r, verbose, log_category, "Initializing {} to {}", value.name, it->second);
				set_value(value, it->second.c_str());
			}
		}

		void set_value(adjustable::value & value, const char * s) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto assign_ptr = [](auto & storage) {
				if (storage.ptr != nullptr)
					*storage.ptr = storage.value;
			};

			const std::string_view view(s);
			switch (value.type) {
				case adjustable::value_type::Int: {
					const auto result = scn::scan_default(view, value.int_storage.value);
					if (!result)
						kengine_assert_failed(r, "{}", result.error().msg());
					assign_ptr(value.int_storage);
					break;
				}
				case adjustable::value_type::Float: {
					const auto result = scn::scan_default(view, value.float_storage.value);
					if (!result)
						kengine_assert_failed(r, "{}", result.error().msg());
					assign_ptr(value.float_storage);
					break;
				}
				case adjustable::value_type::Bool: {
					const auto result = scn::scan_default(view, value.bool_storage.value);
					if (!result)
						kengine_assert_failed(r, "{}", result.error().msg());
					assign_ptr(value.bool_storage);
					break;
				}
				case adjustable::value_type::Color: {
					const auto result = scn::scan_default(view, value.color_storage.value);
					if (!result)
						kengine_assert_failed(r, "{}", result.error().msg());
					assign_ptr(value.color_storage);
					break;
				}
				default: {
					static_assert(magic_enum::enum_count<adjustable::value_type>() == 5); // + 1 for Invalid
					kengine_assert_failed(r, "Unknown adjustable::value type");
					break;
				}
			}
		}

		putils::ini_file load_ini_file() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, log_category, "Loading from " KENGINE_ADJUSTABLE_SAVE_FILE);

			std::ifstream f(KENGINE_ADJUSTABLE_SAVE_FILE);

			putils::ini_file ret;
			f >> ret;
			return ret;
		}

		void save() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, log_category, "Saving to " KENGINE_ADJUSTABLE_SAVE_FILE);

			std::ofstream f(KENGINE_ADJUSTABLE_SAVE_FILE, std::ofstream::trunc);
			if (!f) {
				kengine_assert_failed(r, "Failed to open '" KENGINE_ADJUSTABLE_SAVE_FILE "' with write permissions");
				return;
			}

			putils::ini_file ini;

			for (const auto & [e, comp] : r.view<adjustable>().each()) {
				kengine_logf(r, verbose, log_category, "Adding section {} to INI file", e, comp.section);
				auto & section = ini.sections[comp.section.c_str()];

				for (const auto & value : comp.values) {
					kengine_logf(r, verbose, log_category, "Adding value {} to section {} of INI file", e, value.name);
					auto & ini_value = section.values[value.name.c_str()];

					switch (value.type) {
						case adjustable::value_type::Int: {
							ini_value = fmt::format("{}", value.int_storage.value);
							kengine_logf(r, verbose, log_category, "Adding int value {}", e, ini_value);
							break;
						}
						case adjustable::value_type::Float: {
							ini_value = fmt::format("{}", value.float_storage.value);
							kengine_logf(r, verbose, log_category, "Adding float value {}", e, ini_value);
							break;
						}
						case adjustable::value_type::Bool: {
							ini_value = fmt::format("{}", value.bool_storage.value);
							kengine_logf(r, verbose, log_category, "Adding bool value {}", e, ini_value);
							break;
						}
						case adjustable::value_type::Color: {
							ini_value = fmt::format("{}", putils::to_rgba(value.color_storage.value));
							kengine_logf(r, verbose, log_category, "Adding color value {}", e, ini_value);
							break;
						}
						default: {
							static_assert(magic_enum::enum_count<adjustable::value_type>() == 5); // + 1 for Invalid
							kengine_assert_failed(r, "Unknown adjustable::value type");
							break;
						}
					}
				}
			}

			f << ini;
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		system,
		system::processed
	)
}
