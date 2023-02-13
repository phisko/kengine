#include "instance_helper.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine helpers
#include "kengine/core/helpers/assert_helper.hpp"
#include "kengine/core/helpers/profiling_helper.hpp"

namespace kengine::instance_helper {
	template<typename Comp>
	bool model_has(const entt::registry & r, const data::instance & instance) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_assert(r, instance.model != entt::null);
		return r.all_of<Comp>(instance.model);
	}

	template<typename Comp>
	bool model_has(entt::const_handle instance) noexcept {
		return model_has<Comp>(*instance.registry(), instance.get<data::instance>());
	}

	template<typename Comp>
	const Comp & get_model(const entt::registry & r, const data::instance & instance) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_assert(r, instance.model != entt::null);
		return r.get<Comp>(instance.model);
	}

	template<typename Comp>
	const Comp & get_model(entt::const_handle e) noexcept {
		return get_model<Comp>(*e.registry(), e.get<data::instance>());
	}

	template<typename Comp>
	const Comp * try_get_model(const entt::registry & r, const data::instance & instance) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_assert(r, instance.model != entt::null);
		return r.try_get<Comp>(instance.model);
	}

	template<typename Comp>
	const Comp * try_get_model(entt::const_handle e) noexcept {
		return try_get_model<Comp>(*e.registry(), e.get<data::instance>());
	}
}
