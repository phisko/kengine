#include "system.hpp"

// stl
#include <mutex>
#include <list>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// magic_enum
#include <magic_enum.hpp>

// imgui
#include <imgui.h>

// putils
#include "putils/forward_to.hpp"
#include "putils/lengthof.hpp"
#include "putils/thread_name.hpp"
#include "putils/reflection_helpers/imgui_helper.hpp"

// kengine
#include "kengine/adjustable/data/values.hpp"
#include "kengine/core/data/name.hpp"
#include "kengine/core/log/functions/on_log.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/log/helpers/parse_command_line_severity.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/imgui/helpers/set_context.hpp"
#include "kengine/imgui/tool/data/tool.hpp"
#include "kengine/main_loop/functions/execute.hpp"

namespace kengine::core::log::imgui {
	static constexpr auto log_category = "core_log_imgui";

	struct system {
		const entt::registry & r;
		bool * enabled;

		struct internal_log_event {
			severity message_severity;
			std::string thread;
			std::string category;
			std::string message;
		};

		std::mutex mutex;
		int max_events = 4096;
		std::list<internal_log_event> events;
		std::vector<internal_log_event> filtered_events;

		struct {
			bool severities[magic_enum::enum_count<severity>()];
			std::unordered_map<std::string, severity> category_severities;
			char category_search[4096] = "";
			char thread_search[4096] = "";
		} filters;
		severity_control * control = nullptr;

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;

			e.emplace<on_log>(putils_forward_to_this(log));

			kengine_log(r, log, log_category, "Initializing");

			std::fill(std::begin(filters.severities), std::end(filters.severities), true);

			control = &e.emplace<severity_control>(parse_command_line_severity(r));
			for (int i = 0; i < (int)control->global_severity; ++i)
				filters.severities[i] = false;
			filters.category_severities = control->category_severities;
			e.emplace<main_loop::execute>(putils_forward_to_this(execute));

			e.emplace<core::name>("Log");
			auto & tool = e.emplace<kengine::imgui::tool::tool>();
			enabled = &tool.enabled;

			e.emplace<adjustable::values>() = {
				"Log",
				{
					{ "ImGui max events", &max_events },
				}
			};
		}

		void log(const event & log_event) noexcept {
			KENGINE_PROFILING_SCOPE;

			internal_log_event internal_event{
				log_event.message_severity,
				putils::get_thread_name(),
				log_event.category,
				log_event.message
			};

			if (matches_filters(internal_event)) {
				const std::lock_guard lock(mutex);
				filtered_events.push_back(internal_event);
				events.emplace_back(std::move(internal_event));
				if (events.size() >= max_events)
					events.pop_front();
			}
		}

		void execute(float) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");

			if (!*enabled) {
				kengine_log(r, very_verbose, log_category, "Disabled");
				return;
			}

			if (!kengine::imgui::set_context(r))
				return;

			if (ImGui::Begin("log", enabled)) {
				draw_filters();
				draw_filtered_events();
			}
			ImGui::End();
		}

		void draw_filters() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Drawing filters");

			bool changed = false;
			for (const auto & [log_level, name] : magic_enum::enum_entries<severity>())
				if (ImGui::Checkbox(putils::string<32>("{}", name).c_str(), &filters.severities[(int)log_level])) {
					kengine_logf(r, verbose, log_category, "Filter for {} changed to {}", putils::string<32>(name), filters.severities[(int)log_level]);
					changed = true;

					for (int i = 0; i < putils::lengthof(filters.severities); ++i)
						if (filters.severities[i]) {
							control->global_severity = severity(i);
							break;
						}
				}

			if (putils::reflection::imgui_edit("Categories", filters.category_severities)) {
				control->category_severities.clear();
				for (const auto & [category, severity] : filters.category_severities)
					control->category_severities.emplace(category, severity);
			}

			if (ImGui::InputText("Category", filters.category_search, putils::lengthof(filters.category_search))) {
				kengine_logf(r, verbose, log_category, "Category filter changed to '{}'", filters.category_search);
				changed = true;
			}

			if (ImGui::InputText("Thread", filters.thread_search, putils::lengthof(filters.thread_search))) {
				kengine_logf(r, verbose, log_category, "Thread filter changed to '{}'", filters.thread_search);
				changed = true;
			}

			if (changed)
				update_filtered_events();
		}

		void update_filtered_events() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, log_category, "Updating filtered events");

			const std::lock_guard lock(mutex);
			filtered_events.clear();
			for (const auto & event : events)
				if (matches_filters(event))
					filtered_events.push_back(event);
		}

		bool matches_filters(const internal_log_event & e) noexcept {
			KENGINE_PROFILING_SCOPE;

			const auto passes_severity = [&] {
				if (const auto it = filters.category_severities.find(e.category); it != filters.category_severities.end())
					if (e.message_severity >= it->second)
						return true;
				return filters.severities[int(e.message_severity)];
			};

			if (!passes_severity())
				return false;

			const auto passes_search = [](const std::string & haystack, const char * needle) {
				if (haystack.empty()) {
					if (needle[0])
						return false;
				}
				else {
					if (haystack.find(needle) == std::string::npos)
						return false;
				}
				return true;
			};

			if (!passes_search(e.category, filters.category_search))
				return false;

			if (!passes_search(e.thread, filters.thread_search))
				return false;

			return true;
		}

		void draw_filtered_events() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Drawing filtered events");

			if (ImGui::BeginTable("##logEvents", 4)) {
				ImGui::TableSetupColumn("Severity");
				ImGui::TableSetupColumn("Thread");
				ImGui::TableSetupColumn("Category");
				ImGui::TableSetupColumn("Message");
				ImGui::TableHeadersRow();

				const std::lock_guard lock_guard(mutex);
				for (const auto & event : filtered_events) {
					ImGui::TableNextColumn();
					ImGui::Text("%s", putils::string<1024>(magic_enum::enum_name(event.message_severity)).c_str());
					ImGui::TableNextColumn();
					ImGui::Text("%s", event.thread.c_str());
					ImGui::TableNextColumn();
					ImGui::Text("%s", event.category.c_str());
					ImGui::TableNextColumn();
					ImGui::Text("%s", event.message.c_str());
				}

				ImGui::EndTable();
			}
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system)
}
