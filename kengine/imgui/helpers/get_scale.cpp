#include "get_scale.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine imgui
#include "kengine/imgui/data/scale.hpp"

namespace kengine::imgui {
	float get_scale(const entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, very_verbose, "imgui", "Getting scale");

		float result = 1.f;
		for (const auto & [e, comp] : r.view<scale>().each()) {
			kengine_logf(r, very_verbose, "imgui", "Found modifier [%u] (%f)", e, comp.modifier);
			result *= comp.modifier;
		}

		kengine_logf(r, very_verbose, "imgui", "Final scale: %f", result);
		return result;
	}
}