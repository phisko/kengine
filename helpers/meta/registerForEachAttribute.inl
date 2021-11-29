#include "registerForEachAttribute.hpp"
#include "meta/ForEachAttribute.hpp"
#include "helpers/registerMetaComponentImplementation.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerForEachAttribute() noexcept {
		registerMetaComponentImplementation<meta::ForEachAttribute, Comps...>(
			[](const auto t, Entity & e, auto && func) noexcept {
				using Type = putils_wrapped_type(t);
				auto & comp = e.get<Type>();
				putils::reflection::for_each_attribute(comp,
					[&](const char * name, auto && member) {
						const auto typeIndex = putils::meta::type<putils_typeof(member)>::index;
						const meta::AttributeInfo attribute{
							.name = name,
							.member = &member,
							.size = sizeof(member),
							.type = typeIndex
						};
						func(attribute);
					}
				);
			}
		);
	}
}