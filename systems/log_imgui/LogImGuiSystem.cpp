#include "LogImGuiSystem.hpp"
#include "kengine.hpp"

// stl
#include <mutex>
#include <list>

// magic_enum
#include <magic_enum.hpp>

// imgui
#include <imgui.h>

// putils
#include "thread_name.hpp"
#include "lengthof.hpp"

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
	namespace {
		static struct {
			bool severities[magic_enum::enum_count<kengine::LogSeverity>()];
			char categorySearch[4096] = "";
			char threadSearch[4096] = "";
		} _filters;
	}

	struct LogImGuiSystem {
		struct LogEvent {
			kengine::LogSeverity severity;
			std::string thread;
			std::string category;
			std::string message;
		};

		static void init(Entity & e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Log, "Init", "LogImGuiSystem");

			std::fill(std::begin(_filters.severities), std::end(_filters.severities), true);

			const auto severity = logHelper::parseCommandLineSeverity();
			for (int i = 0; i < (int)severity; ++i)
				_filters.severities[i] = false;

			auto & tool = e.attach<ImGuiToolComponent>();
			_enabled = &tool.enabled;
			e += NameComponent{ "Log" };

			e += functions::Log{ log };
			e += functions::Execute{ execute };

			e += AdjustableComponent{
				"Log", {
					{ "ImGui max events", &_maxEvents }
				}
			};
		}

		static void log(const kengine::LogEvent & event) noexcept {
			KENGINE_PROFILING_SCOPE;

			LogEvent e{
				 event.severity,
				 putils::get_thread_name(),
				 event.category,
				 event.message
			};

			if (matchesFilters(e)) {
				const std::lock_guard lock(_mutex);
				_filteredEvents.push_back(e);
				_events.emplace_back(std::move(e));
				if (_events.size() >= _maxEvents)
					_events.pop_front();
			}
		}

		static void execute(float) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!*_enabled)
				return;

			kengine_log(Verbose, "Execute", "LogImGuiSystem");

			if (ImGui::Begin("Log", _enabled)) {
				drawFilters();
				drawFilteredEvents();
			}
			ImGui::End();
		}

		static void drawFilters() noexcept {
			KENGINE_PROFILING_SCOPE;

			bool changed = false;
			for (const auto & [severity, name] : magic_enum::enum_entries<LogSeverity>())
				if (ImGui::Checkbox(putils::string<32>(name), &_filters.severities[(int)severity]))
					changed = true;

			if (ImGui::InputText("Category", _filters.categorySearch, putils::lengthof(_filters.categorySearch)))
				changed = true;

			if (ImGui::InputText("Thread", _filters.threadSearch, putils::lengthof(_filters.threadSearch)))
				changed = true;

			if (changed)
				updateFilteredEvents();
		}

		static void updateFilteredEvents() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Verbose, "Execute/LogImGuiSystem", "Updating filters");

			const std::lock_guard lock(_mutex);
			_filteredEvents.clear();
			for (const auto & event : _events)
				if (matchesFilters(event))
					_filteredEvents.push_back(event);
		}

		static bool matchesFilters(const LogEvent & e) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!_filters.severities[(int)e.severity])
				return false;

			if (!e.category.empty() && e.category.find(_filters.categorySearch) == std::string::npos)
				return false;

			if (!e.thread.empty() && e.thread.find(_filters.threadSearch) == std::string::npos)
				return false;

			return true;
		}

		static void drawFilteredEvents() noexcept {
			KENGINE_PROFILING_SCOPE;

			if (ImGui::BeginTable("##logEvents", 4)) {
				ImGui::TableSetupColumn("Severity");
				ImGui::TableSetupColumn("Thread");
				ImGui::TableSetupColumn("Category");
				ImGui::TableSetupColumn("Message");
				ImGui::TableHeadersRow();

				std::lock_guard lockGuard(_mutex);
				for (const auto& event : _filteredEvents) {
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

		static inline bool * _enabled;

		static inline std::mutex _mutex;
		static inline int _maxEvents = 4096;
		static inline std::list<LogEvent> _events;
		static inline std::vector<LogEvent> _filteredEvents;
	};

	EntityCreator * LogImGuiSystem() noexcept {
		return LogImGuiSystem::init;
	}
}
