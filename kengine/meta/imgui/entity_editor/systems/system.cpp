#include "system.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// putils
#include "putils/forward_to.hpp"

// kengine
#include "kengine/core/data/name.hpp"
#include "kengine/core/data/selected.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/imgui/helpers/get_scale.hpp"
#include "kengine/imgui/helpers/set_context.hpp"
#include "kengine/imgui/tool/data/tool.hpp"
#include "kengine/main_loop/functions/execute.hpp"
#include "kengine/meta/imgui/helpers/edit_entity.hpp"

namespace kengine::meta::imgui::entity_editor {
	static constexpr auto log_category = "meta_imgui_entity_editor";

	struct system {
		entt::registry & r;
		bool * enabled;

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");

			e.emplace<main_loop::execute>(putils_forward_to_this(execute));

			e.emplace<core::name>("Entities/Editor");
			auto & tool = e.emplace<kengine::imgui::tool::tool>(true);
			enabled = &tool.enabled;
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

			const auto scale = kengine::imgui::get_scale(r);
			for (const auto selected : r.view<core::selected>()) {
				bool open = true;

				ImGui::SetNextWindowSize({ 200.f * scale, 200.f * scale }, ImGuiCond_FirstUseEver);

				const auto name = r.try_get<core::name>(selected);
				const auto window_title =
					name ?
					putils::string<64>("{}##{}", name->name, selected) :
					putils::string<64>("{} Entity editor", selected);
				kengine_logf(r, very_verbose, log_category, "Displaying {} ({})", selected, window_title);

				if (ImGui::Begin(window_title.c_str(), &open, ImGuiWindowFlags_NoSavedSettings))
					if (edit_entity_and_model({ r, selected }))
						kengine_logf(r, log, log_category, "{} was modified", selected);
				ImGui::End();

				if (!open) {
					kengine_logf(r, log, log_category, "De-selecting {}", selected);
					r.remove<core::selected>(selected);
				}
			}
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(system)
}
