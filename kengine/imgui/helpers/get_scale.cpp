#include "get_scale.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/imgui/data/scale.hpp"

namespace kengine::imgui {
	static constexpr auto log_category = "imgui";

	float get_scale(const entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, very_verbose, log_category, "Getting scale");

		float result = 1.f;
		for (const auto & [e, comp] : r.view<scale>().each()) {
			kengine_logf(r, very_verbose, log_category, "Found modifier {} ({})", e, comp.modifier);
			result *= comp.modifier;
		}

		kengine_logf(r, very_verbose, log_category, "Final scale: {}", result);
		return result;
	}
}