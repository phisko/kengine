#include "entity_appears_in_viewport.hpp"

// kengine
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/render/functions/appears_in_viewport.hpp"

namespace kengine::render {
	static constexpr auto log_category = "render";

	bool entity_appears_in_viewport(const entt::registry & r, entt::entity entity, entt::entity viewport_entity) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, very_verbose, log_category, "Checking whether {} appears in viewport {}", entity, viewport_entity);

		const auto wants_to_appear = [&](entt::entity lhs, entt::entity rhs) noexcept {
			if (const auto appears_in_viewport = r.try_get<render::appears_in_viewport>(lhs))
				if (!appears_in_viewport->call(rhs)) {
					kengine_logf(r, very_verbose, log_category, "{} refused", lhs);
					return false;
				}
			return true;
		};

		return wants_to_appear(entity, viewport_entity) && wants_to_appear(viewport_entity, entity);
	}
}