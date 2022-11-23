#include "registerMetaComponentImplementation.hpp"

// entt
#include <entt/entity/registry.hpp>

// meta
#include "meta/for_each.hpp"
#include "meta/fwd.hpp"

// kengine helpers
#include "helpers/typeHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	template<typename Meta, typename... Comps, typename Func>
	void registerMetaComponentImplementation(entt::registry & r, Func && f) noexcept {
		registerMetaComponentImplementationWithPredicate<Meta, std::is_default_constructible, Comps...>(r, FWD(f));
	}

	template<typename Meta, template<typename T> typename Predicate, typename ... Comps, typename Func>
    void registerMetaComponentImplementationWithPredicate(entt::registry & r, Func && f) noexcept {
		KENGINE_PROFILING_SCOPE;

		putils::for_each_type<Comps...>([&](auto t) noexcept {
			using Type = putils_wrapped_type(t);
			if constexpr (!Predicate<Type>()) {
				kengine_logf(r, Warning, "Init/registerMetaComponents", "Skipping %s for %s because predicate is not met", putils::reflection::get_class_name<Meta>(), putils::reflection::get_class_name<Type>());
			} else {
				if constexpr (putils::reflection::has_class_name<Meta>() && putils::reflection::has_class_name<Type>())
					kengine_logf(r, Log, "Init/registerMetaComponents", "Registering %s for %s", putils::reflection::get_class_name<Meta>(), putils::reflection::get_class_name<Type>());
				const auto type = typeHelper::getTypeEntity<Type>(r);
				r.emplace<Meta>(type, [=](auto &&... args) noexcept {
					return f(t, FWD(args)...);
				});
			}
		});
	}
}

