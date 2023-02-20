#include "type_helper.hpp"

// entt
#include <entt/entity/registry.hpp>

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/on_scope_exit.hpp"

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::type_helper {
	template<typename T>
	struct type_entity_tag {};

	template<typename T>
	entt::entity get_type_entity(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		if constexpr (putils::reflection::has_class_name<T>())
			kengine_logf(r, very_verbose, "type_helper", "Getting type entity for %s", putils::reflection::get_class_name<T>());

		if (const auto e = r.view<type_entity_tag<T>>().front(); e != entt::null) {
			if constexpr (putils::reflection::has_class_name<T>())
				kengine_logf(r, very_verbose, "type_helper", "Found existing type entity [%u] for '%s'", e, putils::reflection::get_class_name<T>());
			return e;
		}

		const auto e = r.create();

		if constexpr (putils::reflection::has_class_name<T>())
			kengine_logf(r, verbose, "type_helper", "Initializing type entity [%u] for '%s'", e, putils::reflection::get_class_name<T>());

		r.emplace<type_entity_tag<T>>(e);
		return e;
	};
}