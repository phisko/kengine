#include "imgui_entity_editor.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// putils
#include "putils/forward_to.hpp"

// kengine core
#include "kengine/core/data/name.hpp"
#include "kengine/core/data/selected.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine model_instance
#include "kengine/model_instance/data/instance.hpp"

// kengine imgui
#include "kengine/imgui/helpers/get_scale.hpp"

// kengine imgui/tool
#include "kengine/imgui/tool/data/tool.hpp"

// kengine main_loop
#include "kengine/main_loop/functions/execute.hpp"

// kengine meta/imgui
#include "kengine/meta/imgui/helpers/imgui_helper.hpp"

namespace kengine::systems {
	struct imgui_entity_editor {
		entt::registry & r;
		bool * enabled;

		imgui_entity_editor(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "imgui_entity_editor", "Initializing");

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));

			e.emplace<core::name>("Entities/Editor");
			auto & tool = e.emplace<imgui::tool::tool>(true);
			enabled = &tool.enabled;
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "imgui_entity_editor", "Executing");

			if (!*enabled) {
				kengine_log(r, very_verbose, "imgui_entity_editor", "Disabled");
				return;
			}

			const auto scale = imgui::get_scale(r);
			for (const auto selected : r.view<core::selected>()) {
				bool open = true;

				ImGui::SetNextWindowSize({ 200.f * scale, 200.f * scale }, ImGuiCond_FirstUseEver);

				const auto name = r.try_get<core::name>(selected);
				const auto window_title =
					name ?
					putils::string<64>("%s##[%d]", name->name.c_str(), selected) :
					putils::string<64>("[%d] Entity editor", selected);
				kengine_logf(r, very_verbose, "imgui_entity_editor", "Displaying [%u] (%s)", selected, window_title.c_str());

				if (ImGui::Begin(window_title.c_str(), &open, ImGuiWindowFlags_NoSavedSettings))
					imgui_helper::edit_entity_and_model({ r, selected });
				ImGui::End();

				if (!open) {
					kengine_logf(r, log, "imgui_entity_editor", "De-selecting [%u]", selected);
					r.remove<core::selected>(selected);
				}
			}
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(imgui_entity_editor)
}
