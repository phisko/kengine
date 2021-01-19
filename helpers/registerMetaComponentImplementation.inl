#include "registerMetaComponentImplementation.hpp"
#include "helpers/typeHelper.hpp"
#include "meta/for_each.hpp"
#include "fwd.hpp"

namespace kengine {
	template<typename Meta, typename ... Comps, typename Func>
	void registerMetaComponentImplementation(Func && f) noexcept {
		putils::for_each_type<Comps...>([&](auto t) noexcept {
			using Type = putils_wrapped_type(t);
			auto type = typeHelper::getTypeEntity<Type>();
			type += Meta{
				[=](auto && ... args) noexcept {
					return f(t, FWD(args)...);
				}
			};
		});
	}
}

