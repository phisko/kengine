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
	template<typename Meta, typename... Comps, typename Func>
	void register_meta_component_implementation(entt::registry & r, Func && f) noexcept {
		register_meta_component_implementation_with_predicate<Meta, std::is_default_constructible, Comps...>(r, FWD(f));
	}

	template<typename Meta, template<typename T> typename Predicate, typename... Comps, typename Func>
	void register_meta_component_implementation_with_predicate(entt::registry & r, Func && f) noexcept {
		KENGINE_PROFILING_SCOPE;

		putils::for_each_type<Comps...>([&](auto t) noexcept {
			using Type = putils_wrapped_type(t);
			if constexpr (!Predicate<Type>()) {
				kengine_logf(r, warning, "Init/register_meta_components", "Skipping %s for %s because predicate is not met", putils::reflection::get_class_name<Meta>(), putils::reflection::get_class_name<Type>());
			}
			else {
				if constexpr (putils::reflection::has_class_name<Meta>() && putils::reflection::has_class_name<Type>())
					kengine_logf(r, log, "Init/register_meta_components", "Registering %s for %s", putils::reflection::get_class_name<Meta>(), putils::reflection::get_class_name<Type>());
				const auto type = type_helper::get_type_entity<Type>(r);
				r.emplace<Meta>(type, [=](auto &&... args) noexcept {
					return f(t, FWD(args)...);
				});
			}
		});
	}
}
