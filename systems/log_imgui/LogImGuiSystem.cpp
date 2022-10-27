#include "LogImGuiSystem.hpp"

// stl
#include <mutex>
#include <list>

#include "kengine.hpp"

// kengine data
#include "data/AdjustableComponent.hpp"
#include "data/ImGuiToolComponent.hpp"
#include "data/NameComponent.hpp"

// kengine functions
#include "functions/Log.hpp"
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"

// putils
#include <magic_enum.hpp>
#include "thread_name.hpp"
#include "lengthof.hpp"
#include "imgui.h"

namespace detail {
    struct LogEvent {
        kengine::LogSeverity severity;
        std::string thread;
        std::string category;
        std::string message;
    };
}

static bool * g_enabled;

static struct {
	bool severities[magic_enum::enum_count<kengine::LogSeverity>()];
	char categorySearch[4096] = "";
	char threadSearch[4096] = "";
} g_filters;

static std::mutex g_mutex;
static int g_maxEvents = 4096;
static std::list<detail::LogEvent> g_events;
static std::vector<detail::LogEvent> g_filteredEvents;

namespace kengine {
	EntityCreator * LogImGuiSystem() noexcept {
		struct impl {
			static void init(Entity & e) noexcept {
				kengine_log(Log, "Init", "LogImGuiSystem");

                std::fill(std::begin(g_filters.severities), std::end(g_filters.severities), true);

                const auto severity = logHelper::parseCommandLineSeverity();
                for (int i = 0; i < (int)severity; ++i)
                    g_filters.severities[i] = false;

				auto & tool = e.attach<ImGuiToolComponent>();
				g_enabled = &tool.enabled;
				e += NameComponent{ "Log" };

				e += functions::Log{ log };
				e += functions::Execute{ execute };

				e += AdjustableComponent{
					"Log", {
						{ "ImGui max events", &g_maxEvents }
					}
				};
			}

			static void log(const kengine::LogEvent & event) noexcept {
				detail::LogEvent e{
					 event.severity,
					 putils::get_thread_name(),
					 event.category,
					 event.message
				};

				if (matchesFilters(e)) {
                    const std::lock_guard lock(g_mutex);
					g_filteredEvents.push_back(e);
					g_events.emplace_back(std::move(e));
					if (g_events.size() >= g_maxEvents)
						g_events.pop_front();
				}
			}

			static void execute(float) noexcept {
				if (!*g_enabled)
					return;

				kengine_log(Verbose, "Execute", "LogImGuiSystem");

				if (ImGui::Begin("Log", g_enabled)) {
					drawFilters();
					drawFilteredEvents();
				}
				ImGui::End();
			}

			static void drawFilters() noexcept {
				bool changed = false;
				for (const auto & [severity, name] : magic_enum::enum_entries<LogSeverity>())
					if (ImGui::Checkbox(putils::string<32>(name), &g_filters.severities[(int)severity]))
						changed = true;

				if (ImGui::InputText("Category", g_filters.categorySearch, putils::lengthof(g_filters.categorySearch)))
					changed = true;

				if (ImGui::InputText("Thread", g_filters.threadSearch, putils::lengthof(g_filters.threadSearch)))
					changed = true;

				if (changed)
					updateFilteredEvents();
			}

			static void updateFilteredEvents() noexcept {
				kengine_log(Verbose, "Execute/LogImGuiSystem", "Updating filters");

                const std::lock_guard lock(g_mutex);
				g_filteredEvents.clear();
				for (const auto & event : g_events)
					if (matchesFilters(event))
						g_filteredEvents.push_back(event);
			}

			static bool matchesFilters(const detail::LogEvent & e) noexcept {
				if (!g_filters.severities[(int)e.severity])
					return false;

				if (!e.category.empty() && e.category.find(g_filters.categorySearch) == std::string::npos)
					return false;

				if (!e.thread.empty() && e.thread.find(g_filters.threadSearch) == std::string::npos)
					return false;

				return true;
			}

			static void drawFilteredEvents() noexcept {
				if (ImGui::BeginTable("##logEvents", 4)) {
					ImGui::TableSetupColumn("Severity");
					ImGui::TableSetupColumn("Thread");
					ImGui::TableSetupColumn("Category");
					ImGui::TableSetupColumn("Message");
					ImGui::TableHeadersRow();

                    std::lock_guard lockGuard(g_mutex);
					for (const auto& event : g_filteredEvents) {
						ImGui::TableNextColumn();
						ImGui::Text(putils::string<1024>(magic_enum::enum_name(event.severity)));
						ImGui::TableNextColumn();
						ImGui::Text(event.thread.c_str());
						ImGui::TableNextColumn();
						ImGui::Text(event.category.c_str());
						ImGui::TableNextColumn();
						ImGui::Text(event.message.c_str());
					}

					ImGui::EndTable();
				}
			}
		};

		return impl::init;
	}
}
