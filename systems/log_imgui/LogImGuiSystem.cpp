#include "LogImGuiSystem.hpp"

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
#include "forward_to.hpp"
#include "lengthof.hpp"
#include "thread_name.hpp"

// kengine data
#include "data/AdjustableComponent.hpp"
#include "data/ImGuiToolComponent.hpp"
#include "data/NameComponent.hpp"

// kengine functions
#include "functions/Log.hpp"
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	struct LogImGuiSystem {
		const entt::registry & r;
		bool * enabled;

		struct InternalLogEvent {
			kengine::LogSeverity severity;
			std::string thread;
			std::string category;
			std::string message;
		};

		std::mutex mutex;
		int maxEvents = 4096;
		std::list<InternalLogEvent> events;
		std::vector<InternalLogEvent> filteredEvents;

		struct {
			bool severities[magic_enum::enum_count<kengine::LogSeverity>()];
			char categorySearch[4096] = "";
			char threadSearch[4096] = "";
		} filters;

		LogImGuiSystem(entt::handle e) noexcept
			: r(*e.registry())
		{
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "LogImGuiSystem");

			std::fill(std::begin(filters.severities), std::end(filters.severities), true);

			const auto severity = logHelper::parseCommandLineSeverity(r);
			for (int i = 0; i < (int)severity; ++i)
				filters.severities[i] = false;

			e.emplace<functions::Log>(putils_forward_to_this(log));
			e.emplace<functions::Execute>(putils_forward_to_this(execute));

			e.emplace<NameComponent>("Log");
			auto & tool = e.emplace<ImGuiToolComponent>();
			enabled = &tool.enabled;

			e.emplace<AdjustableComponent>() = {
				"Log", {
					{ "ImGui max events", &maxEvents }
				}
			};
		}

		void log(const kengine::LogEvent & event) noexcept {
			KENGINE_PROFILING_SCOPE;

			InternalLogEvent e{
				 event.severity,
				 putils::get_thread_name(),
				 event.category,
				 event.message
			};

			if (matchesFilters(e)) {
				const std::lock_guard lock(mutex);
				filteredEvents.push_back(e);
				events.emplace_back(std::move(e));
				if (events.size() >= maxEvents)
					events.pop_front();
			}
		}

		void execute(float) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!*enabled)
				return;

			kengine_log(r, Verbose, "Execute", "LogImGuiSystem");

			if (ImGui::Begin("Log", enabled)) {
				drawFilters();
				drawFilteredEvents();
			}
			ImGui::End();
		}

		void drawFilters() noexcept {
			KENGINE_PROFILING_SCOPE;

			bool changed = false;
			for (const auto & [severity, name] : magic_enum::enum_entries<LogSeverity>())
				if (ImGui::Checkbox(putils::string<32>(name).c_str(), &filters.severities[(int)severity]))
					changed = true;

			if (ImGui::InputText("Category", filters.categorySearch, putils::lengthof(filters.categorySearch)))
				changed = true;

			if (ImGui::InputText("Thread", filters.threadSearch, putils::lengthof(filters.threadSearch)))
				changed = true;

			if (changed)
				updateFilteredEvents();
		}

		void updateFilteredEvents() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Verbose, "Execute/LogImGuiSystem", "Updating filters");

			const std::lock_guard lock(mutex);
			filteredEvents.clear();
			for (const auto & event : events)
				if (matchesFilters(event))
					filteredEvents.push_back(event);
		}

		bool matchesFilters(const InternalLogEvent & e) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!filters.severities[(int)e.severity])
				return false;

			if (!e.category.empty() && e.category.find(filters.categorySearch) == std::string::npos)
				return false;

			if (!e.thread.empty() && e.thread.find(filters.threadSearch) == std::string::npos)
				return false;

			return true;
		}

		void drawFilteredEvents() noexcept {
			KENGINE_PROFILING_SCOPE;

			if (ImGui::BeginTable("##logEvents", 4)) {
				ImGui::TableSetupColumn("Severity");
				ImGui::TableSetupColumn("Thread");
				ImGui::TableSetupColumn("Category");
				ImGui::TableSetupColumn("Message");
				ImGui::TableHeadersRow();

				std::lock_guard lockGuard(mutex);
				for (const auto& event : filteredEvents) {
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

	void addLogImGuiSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<LogImGuiSystem>(e);
	}
}
