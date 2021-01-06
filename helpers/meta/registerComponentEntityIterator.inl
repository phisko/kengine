#include "registerComponentEntityIterator.hpp"

#include "kengine.hpp"
#include "meta/ForEachEntity.hpp"
#include "helpers/typeHelper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerComponentEntityIterator() noexcept {
		putils::for_each_type<Comps...>([](auto t) noexcept {
			using Type = putils_wrapped_type(t);

			auto type = typeHelper::getTypeEntity<Type>();

			type += meta::ForEachEntity{
				[](auto && func) noexcept {
					using Type = putils_wrapped_type(t);
					for (auto [e, comp] : entities.with<Type>())
						func(e);
				}
			};

			type += meta::ForEachEntityWithout{
				[](auto && func) noexcept {
					using Type = putils_wrapped_type(t);
					for (auto [e, notComp] : entities.with<no<Type>>())
						func(e);
				}
			};
		});
	}
}
