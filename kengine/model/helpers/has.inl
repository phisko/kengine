#include "has.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/core/assert/helpers/kengine_assert.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::model {
	template<typename Comp>
	bool has(const entt::registry & r, const instance & instance) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_assert(r, instance.model != entt::null);
		return r.all_of<Comp>(instance.model);
	}

	template<typename Comp>
	bool has(entt::const_handle instance_entity) noexcept {
		return has<Comp>(*instance_entity.registry(), instance_entity.get<instance>());
	}
}
