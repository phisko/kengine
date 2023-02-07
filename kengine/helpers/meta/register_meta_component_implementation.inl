#include "register_meta_component_implementation.hpp"

// entt
#include <entt/entity/registry.hpp>

// meta
#include "putils/meta/for_each.hpp"
#include "putils/meta/fwd.hpp"

// kengine helpers
#include "kengine/helpers/type_helper.hpp"
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename Meta, typename... Comps>
	void register_meta_component_implementation(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, verbose, "register_meta_component_implementation", "Registering %s for types", putils::reflection::get_class_name<Meta>());

		putils::for_each_type<Comps...>([&](auto t) noexcept {
			using Type = putils_wrapped_type(t);

			constexpr auto can_log = putils::reflection::has_class_name<Meta>() && putils::reflection::has_class_name<Type>();

			using implementation = meta_component_implementation<Meta, Type>;
			if constexpr (implementation::value) {
				if constexpr (can_log)
					kengine_logf(r, verbose, "register_meta_component_implementation", "Registering %s for %s", putils::reflection::get_class_name<Meta>(), putils::reflection::get_class_name<Type>());

				const auto type = type_helper::get_type_entity<Type>(r);
				r.emplace<Meta>(type, implementation::function);
			}
			else if constexpr (can_log)
				kengine_logf(r, verbose, "register_meta_component_implementation", "Skipping %s registration for %s (conditions not met)", putils::reflection::get_class_name<Meta>(), putils::reflection::get_class_name<Type>());
		});
	}
}