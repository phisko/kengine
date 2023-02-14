#include "log_imgui.hpp"

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

// kengine core
#include "kengine/core/data/name.hpp"
#include "kengine/core/functions/log.hpp"
#include "kengine/core/helpers/log_helper.hpp"
#include "kengine/core/helpers/profiling_helper.hpp"

// kengine adjustable
#include "kengine/adjustable/data/adjustable.hpp"

// kengine imgui/imgui_tool
#include "kengine/imgui/tool/data/imgui_tool.hpp"

// kengine main_loop
#include "kengine/main_loop/functions/execute.hpp"

namespace kengine::systems {
	struct log_imgui {
		const entt::registry & r;
		bool * enabled;

		struct internal_log_event {
			kengine::log_severity severity;
			std::string thread;
			std::string category;
			std::string message;
		};

		std::mutex mutex;
		int max_events = 4096;
		std::list<internal_log_event> events;
		std::vector<internal_log_event> filtered_events;

		struct {
			bool severities[magic_enum::enum_count<kengine::log_severity>()];
			std::unordered_map<std::string, log_severity> category_severities;
			char category_search[4096] = "";
			char thread_search[4096] = "";
		} filters;

		log_imgui(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "log_imgui", "Initializing");

			std::fill(std::begin(filters.severities), std::end(filters.severities), true);

			auto command_line_severity = log_helper::parse_command_line_severity(r);
			for (int i = 0; i < (int)command_line_severity.global_severity; ++i)
				filters.severities[i] = false;
			filters.category_severities = std::move(command_line_severity.category_severities);

			e.emplace<functions::log>(putils_forward_to_this(log));
			e.emplace<functions::execute>(putils_forward_to_this(execute));

			e.emplace<data::name>("Log");
			auto & tool = e.emplace<data::imgui_tool>();
			enabled = &tool.enabled;

			e.emplace<data::adjustable>() = {
				"Log",
				{
					{ "ImGui max events", &max_events },
				}
			};
		}

		void log(const kengine::log_event & event) noexcept {
			KENGINE_PROFILING_SCOPE;

			internal_log_event e{
				event.severity,
				putils::get_thread_name(),
				event.category,
				event.message
			};

			if (matches_filters(e)) {
				const std::lock_guard lock(mutex);
				filtered_events.push_back(e);
				events.emplace_back(std::move(e));
				if (events.size() >= max_events)
					events.pop_front();
			}
		}

		void execute(float) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "log_imgui", "Executing");

			if (!*enabled) {
				kengine_log(r, very_verbose, "log_imgui", "Disabled");
				return;
			}

			if (ImGui::Begin("log", enabled)) {
				draw_filters();
				draw_filtered_events();
			}
			ImGui::End();
		}

		void draw_filters() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "log_imgui", "Drawing filters");

			bool changed = false;
			for (const auto & [severity, name] : magic_enum::enum_entries<log_severity>())
				if (ImGui::Checkbox(putils::string<32>(name).c_str(), &filters.severities[(int)severity])) {
					kengine_logf(r, verbose, "log_imgui", "Filter for %s changed to %d", putils::string<32>(name).c_str(), filters.severities[(int)severity]);
					changed = true;
				}

			putils::reflection::imgui_edit("Categories", filters.category_severities);
			for (const auto & [category, severity] : filters.category_severities)
				log_helper::set_minimum_log_severity(r, severity);

			if (ImGui::InputText("Category", filters.category_search, putils::lengthof(filters.category_search))) {
				kengine_logf(r, verbose, "log_imgui", "Category filter changed to '%s'", filters.category_search);
				changed = true;
			}

			if (ImGui::InputText("Thread", filters.thread_search, putils::lengthof(filters.thread_search))) {
				kengine_logf(r, verbose, "log_imgui", "Thread filter changed to '%s'", filters.thread_search);
				changed = true;
			}

			if (changed)
				update_filtered_events();
		}

		void update_filtered_events() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, "log_imgui", "Updating filtered events");

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
					if (e.severity >= it->second)
						return true;
				return filters.severities[int(e.severity)];
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
			kengine_log(r, very_verbose, "log_imgui", "Drawing filtered events");

			if (ImGui::BeginTable("##logEvents", 4)) {
				ImGui::TableSetupColumn("Severity");
				ImGui::TableSetupColumn("Thread");
				ImGui::TableSetupColumn("Category");
				ImGui::TableSetupColumn("Message");
				ImGui::TableHeadersRow();

				const std::lock_guard lock_guard(mutex);
				for (const auto & event : filtered_events) {
					ImGui::TableNextColumn();
					ImGui::Text("%s", putils::string<1024>(magic_enum::enum_name(event.severity)).c_str());
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

	DEFINE_KENGINE_SYSTEM_CREATOR(log_imgui)
}
