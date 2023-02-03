#include "type_helper.hpp"

// entt
#include <entt/entity/registry.hpp>

// reflection
#include "putils/reflection.hpp"

// putils
#include "putils/on_scope_exit.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::type_helper {
	template<typename T>
	struct type_entity_tag {};

	template<typename T>
	entt::entity get_type_entity(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		if (const auto e = r.view<type_entity_tag<T>>().front(); e != entt::null) {
			if constexpr (putils::reflection::has_class_name<T>())
				kengine_logf(r, verbose, "type_helper", "Found existing type entity %zu for '%s'", e, putils::reflection::get_class_name<T>());
			return e;
		}

		const auto e = r.create();

		if constexpr (putils::reflection::has_class_name<T>())
			kengine_logf(r, log, "type_helper", "Initializing type entity %zu for '%s'", e, putils::reflection::get_class_name<T>());

		r.emplace<type_entity_tag<T>>(e);
		return e;
	};
}