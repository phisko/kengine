#include "registerMetaComponentImplementation.hpp"

// meta
#include "meta/for_each.hpp"
#include "meta/fwd.hpp"

// kengine helpers
#include "helpers/typeHelper.hpp"
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	template<typename Meta, typename ... Comps, typename Func>
	void registerMetaComponentImplementation(Func && f) noexcept {
        registerMetaComponentImplementationWithPredicate<Meta, std::is_default_constructible, Comps...>(FWD(f));
    }

    template<typename Meta, template<typename T> typename Predicate, typename ... Comps, typename Func>
    void registerMetaComponentImplementationWithPredicate(Func && f) noexcept {
		KENGINE_PROFILING_SCOPE;

        putils::for_each_type<Comps...>([&](auto t) noexcept {
            using Type = putils_wrapped_type(t);
            if constexpr (!Predicate<Type>()) {
                kengine_logf(Warning, "Init/registerMetaComponents", "Skipping %s for %s because predicate is not met", putils::reflection::get_class_name<Meta>(), putils::reflection::get_class_name<Type>());
            }
            else {
                if constexpr (putils::reflection::has_class_name<Meta>() && putils::reflection::has_class_name<Type>())
                    kengine_logf(Log, "Init/registerMetaComponents", "Registering %s for %s", putils::reflection::get_class_name<Meta>(), putils::reflection::get_class_name<Type>());
                auto type = typeHelper::getTypeEntity<Type>();
                type += Meta{
                    [=](auto && ... args) noexcept {
                        return f(t, FWD(args)...);
                    }
                };
            }
        });

    }
}

