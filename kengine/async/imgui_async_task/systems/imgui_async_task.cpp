#include "imgui_async_task.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// putils
#include "putils/forward_to.hpp"

// kengine core
#include "kengine/core/data/name.hpp"
#include "kengine/core/helpers/log_helper.hpp"
#include "kengine/core/helpers/profiling_helper.hpp"

// kengine async
#include "kengine/async/data/async_task.hpp"

// kengine imgui/imgui_tool
#include "kengine/imgui/tool/data/imgui_tool.hpp"

// kengine main_loop
#include "kengine/main_loop/functions/execute.hpp"

namespace kengine::systems {
	struct imgui_async_task {
		entt::registry & r;
		bool * enabled = nullptr;

		imgui_async_task(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "imgui_async_task", "Initializing");

			e.emplace<data::name>("Async tasks");
			auto & tool = e.emplace<data::imgui_tool>();
			enabled = &tool.enabled;

			e.emplace<functions::execute>(putils_forward_to_this(execute));
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "imgui_async_task", "Executing");

			if (!*enabled) {
				kengine_log(r, very_verbose, "imgui_async_task", "Disabled");
				return;
			}

			if (ImGui::Begin("Async tasks", enabled)) {
				if (ImGui::BeginTable("Tasks", 2)) {
					ImGui::TableSetupColumn("Name");
					ImGui::TableSetupColumn("Time running");
					ImGui::TableHeadersRow();

					const auto now = std::chrono::system_clock::now();
					for (const auto & [e, task] : r.view<async::task>().each()) {
						kengine_logf(r, very_verbose, "imgui_async_task", "Found async task %s", task.name.c_str());

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

	DEFINE_KENGINE_SYSTEM_CREATOR(imgui_async_task)
}
