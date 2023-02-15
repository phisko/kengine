#include "imgui_helper.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine core
#include "kengine/core/helpers/log_helper.hpp"
#include "kengine/core/helpers/profiling_helper.hpp"

// kengine imgui
#include "kengine/imgui/data/imgui_scale.hpp"

namespace kengine::imgui_helper {
	float get_scale(const entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, very_verbose, "imgui", "Getting scale");

		float scale = 1.f;
		for (const auto & [e, comp] : r.view<data::imgui_scale>().each()) {
			kengine_logf(r, very_verbose, "imgui", "Found modifier [%u] (%f)", e, comp.scale);
			scale *= comp.scale;
		}

		kengine_logf(r, very_verbose, "imgui", "Final scale: %f", scale);
		return scale;
	}
}