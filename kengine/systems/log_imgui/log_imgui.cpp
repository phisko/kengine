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

// kengine data
#include "kengine/data/adjustable.hpp"
#include "kengine/data/imgui_tool.hpp"
#include "kengine/data/name.hpp"

// kengine functions
#include "kengine/functions/log.hpp"
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

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
			char category_search[4096] = "";
			char thread_search[4096] = "";
		} filters;

		log_imgui(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "Init", "systems/log_imgui");

			std::fill(std::begin(filters.severities), std::end(filters.severities), true);

			const auto severity = log_helper::parse_command_line_severity(r);
			for (int i = 0; i < (int)severity; ++i)
				filters.severities[i] = false;

			e.emplace<functions::log>(putils_forward_to_this(log));
			e.emplace<functions::execute>(putils_forward_to_this(execute));

			e.emplace<data::name>("log");
			auto & tool = e.emplace<data::imgui_tool>();
			enabled = &tool.enabled;

			e.emplace<data::adjustable>() = {
				"log",
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

			if (!*enabled)
				return;

			kengine_log(r, verbose, "execute", "log_imgui");

			if (ImGui::Begin("log", enabled)) {
				draw_filters();
				draw_filtered_events();
			}
			ImGui::End();
		}

		void draw_filters() noexcept {
			KENGINE_PROFILING_SCOPE;

			bool changed = false;
			for (const auto & [severity, name] : magic_enum::enum_entries<log_severity>())
				if (ImGui::Checkbox(putils::string<32>(name).c_str(), &filters.severities[(int)severity]))
					changed = true;

			if (ImGui::InputText("Category", filters.category_search, putils::lengthof(filters.category_search)))
				changed = true;

			if (ImGui::InputText("Thread", filters.thread_search, putils::lengthof(filters.thread_search)))
				changed = true;

			if (changed)
				update_filtered_events();
		}

		void update_filtered_events() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, verbose, "execute/log_imgui", "Updating filters");

			const std::lock_guard lock(mutex);
			filtered_events.clear();
			for (const auto & event : events)
				if (matches_filters(event))
					filtered_events.push_back(event);
		}

		bool matches_filters(const internal_log_event & e) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!filters.severities[(int)e.severity])
				return false;

			if (!e.category.empty() && e.category.find(filters.category_search) == std::string::npos)
				return false;

			if (!e.thread.empty() && e.thread.find(filters.thread_search) == std::string::npos)
				return false;

			return true;
		}

		void draw_filtered_events() noexcept {
			KENGINE_PROFILING_SCOPE;

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

	void add_log_imgui(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<log_imgui>(e);
	}
}
