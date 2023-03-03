#pragma once

#include "register_storage.hpp"

// entt
#include <entt/entity/registry.hpp>

// meta
#include "putils/meta/for_each.hpp"

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::meta {
	template<typename... Comps>
	void register_storage(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, verbose, "meta", "Registering storage");

		putils::for_each_type<Comps...>([&](auto && t) noexcept {
			using type = putils_wrapped_type(t);
			kengine_logf(r, verbose, "meta", "Pre-instantiating storage for {}", putils::reflection::get_class_name<type>());
			r.storage<type>(); // Pre-instantiate the component pool
		});
	}

	template<typename... Comps>
	bool is_storage_registered(const entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, very_verbose, "meta", "Checking storage existence");
		const bool any_failed = putils::for_each_type<Comps...>([&](auto && t) noexcept {
			using type = putils_wrapped_type(t);
			return r.storage(entt::type_hash<type>::value()) == nullptr;
		});
		return !any_failed;
	}
}
