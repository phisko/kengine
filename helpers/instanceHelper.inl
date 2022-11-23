#include "instanceHelper.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine helpers
#include "helpers/assertHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::instanceHelper {
	template<typename Comp>
	bool modelHas(const entt::registry & r, const InstanceComponent & instance) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_assert(r, instance.model != entt::null);
		return r.all_of<Comp>(instance.model);
	}

	template<typename Comp>
	bool modelHas(entt::const_handle instance) noexcept {
		return modelHas<Comp>(*instance.registry(), instance.get<InstanceComponent>());
	}

	template<typename Comp>
	const Comp & getModel(const entt::registry & r, const InstanceComponent & instance) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_assert(r, instance.model != entt::null);
		return r.get<Comp>(instance.model);
	}

	template<typename Comp>
	const Comp & getModel(entt::const_handle e) noexcept {
		return getModel<Comp>(*e.registry(), e.get<InstanceComponent>());
	}

	template<typename Comp>
	const Comp * tryGetModel(const entt::registry & r, const InstanceComponent & instance) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_assert(r, instance.model != entt::null);
		return r.try_get<Comp>(instance.model);
	}

	template<typename Comp>
	const Comp * tryGetModel(entt::const_handle e) noexcept {
		return tryGetModel<Comp>(*e.registry(), e.get<InstanceComponent>());
	}
}
