#include "set_context.hpp"

// imgui
#include <imgui.h>

// entt
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/imgui/data/context.hpp"

namespace kengine::imgui {
	static constexpr auto log_category = "imgui";

	bool set_context(const entt::registry & r, get_context_callback_type * get_context_callback, set_context_callback_type * set_context_callback) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, very_verbose, log_category, "Setting context");

		if (get_context_callback()) {
			kengine_log(r, very_verbose, log_category, "Context already set");
			return true;
		}

		for (const auto & [e, imgui_context] : r.view<context>().each()) {
			kengine_logf(r, verbose, log_category, "Found context %p in [%u]", imgui_context.ptr, e);
			set_context_callback(static_cast<ImGuiContext *>(imgui_context.ptr));
			return true;
		}

		kengine_log(r, verbose, log_category, "No context found");
		return false;
	}
}