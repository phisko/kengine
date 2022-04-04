#include "registerMetaComponentImplementation.hpp"
#include "helpers/typeHelper.hpp"
#include "helpers/logHelper.hpp"
#include "meta/for_each.hpp"
#include "meta/fwd.hpp"

namespace kengine {
	template<typename Meta, typename ... Comps, typename Func>
	void registerMetaComponentImplementation(Func && f) noexcept {
		putils::for_each_type<Comps...>([&](auto t) noexcept {
			using Type = putils_wrapped_type(t);
			if constexpr (putils::reflection::has_class_name<Meta>() && putils::reflection::has_class_name<Type>())
				kengine_logf(Log, "Init/registerMetaComponents", "Registering %s for %s", putils::reflection::get_class_name<Meta>(), putils::reflection::get_class_name<Type>());
			auto type = typeHelper::getTypeEntity<Type>();
			type += Meta{
				[=](auto && ... args) noexcept {
					return f(t, FWD(args)...);
				}
			};
		});
	}
}

