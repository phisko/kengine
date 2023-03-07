#include "system.hpp"

// stl
#include <map>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// magic_enum
#include <magic_enum.hpp>

// imgui
#include <imgui.h>

// putils
#include "putils/forward_to.hpp"
#include "putils/split.hpp"

// kengine
#include "kengine/adjustable/data/values.hpp"
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/data/name.hpp"
#include "kengine/core/helpers/new_entity_processor.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/imgui/helpers/set_context.hpp"
#include "kengine/imgui/tool/data/tool.hpp"
#include "kengine/main_loop/functions/execute.hpp"

namespace kengine::adjustable::imgui {
	static constexpr auto log_category = "adjustable_imgui";

	struct system {
		entt::registry & r;

		bool * enabled;

		struct section {
			using section_map = std::map<std::string, section>;
			section_map subsections;

			struct entry {
				entt::entity e = entt::null;
				values * component = nullptr;
				std::vector<bool> values_pass_search; // Indexed by values->values
			};

			std::vector<entry> entries;
			bool passes_search = true;
		};
		section root_section;

		// Remove section from ImGui tree when adjustable is destroyed
		const entt::scoped_connection connection = r.on_destroy<values>().connect<&system::on_destroy_adjustable>(this);

		// Add new section to ImGui tree when adjustable is created
		struct processed {};
		kengine::new_entity_processor<processed, values> processor{ r, putils_forward_to_this(on_construct_adjustable) };

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
		using string = values::string;
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
				entry.values_pass_search.resize(entry.component->entries.size());

				size_t index = 0;
				for (const auto & value : entry.component->entries) {
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
					for (auto & value : entry.component->entries) {
						if (entry.values_pass_search[i])
							draw(entry.e, value);
						++i;
					}
				}

				for (const auto & [subsection_name, subsection] : section.subsections)
					display_menu_entry(subsection_name.c_str(), subsection);

				ImGui::TreePop();
			}
		}

		void draw(entt::entity e, values::value & value) noexcept {
			KENGINE_PROFILING_SCOPE;

			ImGui::Columns(2);
			ImGui::Text("%s", value.name.c_str());
			ImGui::NextColumn();

			bool changed = false;

			switch (value.type) {
				case values::value_type::Int: {
					auto & s = value.int_storage;
					if (value.get_enum_names != nullptr)
						changed = ImGui::Combo(string("##{}", value.name).c_str(), s.ptr != nullptr ? s.ptr : &s.value, value.get_enum_names(), (int)value.enum_count);
					else {
						ImGui::PushItemWidth(-1.f);
						auto val = s.ptr != nullptr ? *s.ptr : s.value;
						if (ImGui::InputInt((string("##") + value.name).c_str(), &val, 1, 100, ImGuiInputTextFlags_EnterReturnsTrue)) {
							changed = true;
							s.value = val;
							if (s.ptr != nullptr)
								*s.ptr = val;
						}
						ImGui::PopItemWidth();
					}
					break;
				}
				case values::value_type::Float: {
					auto & s = value.float_storage;
					ImGui::PushItemWidth(-1.f);
					auto val = s.ptr != nullptr ? *s.ptr : s.value;
					if (ImGui::InputFloat((string("##") + value.name).c_str(), &val, 0.f, 0.f, "%.6f", ImGuiInputTextFlags_EnterReturnsTrue)) {
						changed = true;
						s.value = val;
						if (s.ptr != nullptr)
							*s.ptr = val;
					}
					ImGui::PopItemWidth();
					break;
				}
				case values::value_type::Bool: {
					auto & s = value.bool_storage;
					changed = ImGui::Checkbox((string("##") + value.name).c_str(), s.ptr != nullptr ? s.ptr : &s.value);
					if (s.ptr != nullptr)
						s.value = *s.ptr;
					break;
				}
				case values::value_type::Color: {
					auto & s = value.color_storage;
					const auto color = s.ptr != nullptr ? s.ptr->attributes : s.value.attributes;
					if (ImGui::ColorButton((string("##") + value.name).c_str(), ImVec4(color[0], color[1], color[2], color[3])))
						ImGui::OpenPopup("color picker popup");

					if (ImGui::BeginPopup("color picker popup")) {
						changed = ImGui::ColorPicker4(value.name.c_str(), color);
						ImGui::EndPopup();
					}
					if (s.ptr != nullptr)
						s.value = *s.ptr;
					break;
				}
				default: {
					static_assert(magic_enum::enum_count<values::value_type>() == 5); // + 1 for Invalid
					kengine_assert_failed(r, "Unknown values::value type");
					break;
				}
			}

			// Notify other systems that the value changed
			if (changed)
				r.patch<values>(e);

			ImGui::Columns();
		}

		void on_construct_adjustable(entt::entity e, values & comp) noexcept {
			KENGINE_PROFILING_SCOPE;

			section * current_section = &root_section;
			const auto section_names = putils::split(comp.section.c_str(), '/');
			for (const auto & section_name : section_names)
				current_section = &current_section->subsections[section_name];

			section::entry entry;
			entry.e = e;
			entry.component = &comp;
			current_section->entries.push_back(std::move(entry));

			search_out_of_date = true;
		}

		void on_destroy_adjustable(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & comp = r.get<values>(e);
			kengine_logf(r, verbose, log_category, "Adjustable destroyed in {} (section {})", e, comp.section);
			remove_adjustable_from_section(comp, root_section);
		}

		bool remove_adjustable_from_section(const values & comp, section & section) noexcept {
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
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		system,
		system::processed
	)
}
