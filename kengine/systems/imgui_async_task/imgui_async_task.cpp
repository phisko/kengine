#include "imgui_async_task.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// putils
#include "putils/forward_to.hpp"

// kengine data
#include "kengine/data/async_task.hpp"
#include "kengine/data/imgui_tool.hpp"
#include "kengine/data/name.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::systems {
	struct imgui_async_task {
		entt::registry & r;
		bool * enabled = nullptr;

		imgui_async_task(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "Init", "systems/imgui_async_task");

			e.emplace<data::name>("Async tasks");
			auto & tool = e.emplace<data::imgui_tool>();
			enabled = &tool.enabled;

			e.emplace<functions::execute>(putils_forward_to_this(execute));
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!*enabled)
				return;
			kengine_log(r, verbose, "execute", "imgui_async_task");

			if (ImGui::Begin("Async tasks", enabled)) {
				if (ImGui::BeginTable("Tasks", 2)) {
					ImGui::TableSetupColumn("Name");
					ImGui::TableSetupColumn("Time running");
					ImGui::TableHeadersRow();

					const auto now = std::chrono::system_clock::now();
					for (const auto & [e, async_task] : r.view<data::async_task>().each()) {
						ImGui::TableNextRow();

						ImGui::TableNextColumn();
						ImGui::Text("%s", async_task.name.c_str());

						ImGui::TableNextColumn();
						const auto time_since_start = now - async_task.start;
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
