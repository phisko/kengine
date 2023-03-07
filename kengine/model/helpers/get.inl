#include "get_model.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::instance {
	template<typename Comp>
	const Comp & get_model(const entt::registry & r, const instance & instance) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_assert(r, instance.model != entt::null);
		return r.get<Comp>(instance.model);
	}

	template<typename Comp>
	const Comp & get_model(entt::const_handle e) noexcept {
		return get_model<Comp>(*e.registry(), e.get<instance>());
	}
}