#include "system.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// putils
#include "putils/forward_to.hpp"

// kengine
#include "kengine/async/data/task.hpp"
#include "kengine/core/data/name.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/imgui/helpers/set_context.hpp"
#include "kengine/imgui/tool/data/tool.hpp"
#include "kengine/main_loop/functions/execute.hpp"

namespace kengine::async::imgui {
	static constexpr auto log_category = "async_imgui";

	struct system {
		entt::registry & r;
		bool * enabled = nullptr;

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			e.emplace<core::name>("Async tasks");
			auto & tool = e.emplace<kengine::imgui::tool::tool>();
			enabled = &tool.enabled;

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");

			if (!*enabled) {
				kengine_log(r, very_verbose, log_category, "Disabled");
				return;
			}

			if (!kengine::imgui::set_context(r))
				return;

			if (ImGui::Begin("Async tasks", enabled)) {
				if (ImGui::BeginTable("Tasks", 2)) {
					ImGui::TableSetupColumn("Name");
					ImGui::TableSetupColumn("Time running");
					ImGui::TableHeadersRow();

					const auto now = std::chrono::system_clock::now();
					for (const auto & [e, task] : r.view<async::task>().each()) {
						kengine_logf(r, very_verbose, log_category, "Found async task {}", task.name);

						ImGui::TableNextRow();

						ImGui::TableNextColumn();
						ImGui::Text("%s", task.name.c_str());

						ImGui::TableNextColumn();
						const auto time_since_start = now - task.start;
						const auto seconds = std::chrono::duration<float>(time_since_start);
						ImGui::Text("%f", seconds.count());
					}
					ImGui::EndTable();
				}
			}
			ImGui::End();
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system)
}
