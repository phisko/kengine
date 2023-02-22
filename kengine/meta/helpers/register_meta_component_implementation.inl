#include "register_meta_component_implementation.hpp"

// entt
#include <entt/entity/registry.hpp>

// meta
#include "putils/meta/for_each.hpp"
#include "putils/meta/fwd.hpp"

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine meta
#include "kengine/meta/helpers/get_type_entity.hpp"

namespace kengine::meta {
	template<typename Meta, typename... Comps>
	void register_meta_component_implementation(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, verbose, "meta", "Registering %s for types", putils::reflection::get_class_name<Meta>());

		putils::for_each_type<Comps...>([&](auto t) noexcept {
			using Type = putils_wrapped_type(t);

			constexpr auto can_log = putils::reflection::has_class_name<Meta>() && putils::reflection::has_class_name<Type>();

			using implementation = meta_component_implementation<Meta, Type>;
			if constexpr (implementation::value) {
				if constexpr (can_log)
					kengine_logf(r, verbose, "meta", "Registering %s for %s", putils::reflection::get_class_name<Meta>(), putils::reflection::get_class_name<Type>());

				const auto type = get_type_entity<Type>(r);
				r.emplace<Meta>(type, implementation::function);
			}
			else if constexpr (can_log)
				kengine_logf(r, verbose, "meta", "Skipping %s registration for %s (conditions not met)", putils::reflection::get_class_name<Meta>(), putils::reflection::get_class_name<Type>());
		});
	}
}