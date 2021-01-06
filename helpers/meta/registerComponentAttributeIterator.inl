#include "registerComponentAttributeIterator.hpp"

#include "kengine.hpp"
#include "meta/ForEachAttribute.hpp"
#include "helpers/typeHelper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerComponentAttributeIterator() noexcept {
		putils::for_each_type<Comps...>([](auto t) noexcept {
			using Type = putils_wrapped_type(t);

			auto type = typeHelper::getTypeEntity<Type>();

			type += meta::ForEachAttribute{
				[](Entity & e, auto && func) noexcept {
					using Type = putils_wrapped_type(t);
					auto & comp = e.get<Type>();
					putils::reflection::for_each_attribute(comp,
						[&](const char * name, auto && member) {
							const auto typeIndex = putils::meta::type<putils_typeof(member)>::index;
							func(name, &member, typeIndex);
						}
					);
				}
			};
		});
	}
}