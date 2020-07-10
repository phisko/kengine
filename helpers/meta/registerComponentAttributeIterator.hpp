#pragma once

#include "EntityManager.hpp"
#include "meta/ForEachAttribute.hpp"
#include "helpers/typeHelper.hpp"

namespace kengine {
	template<typename Comp>
	void registerComponentAttributeIterator(EntityManager & em) {
		auto type = typeHelper::getTypeEntity<Comp>(em);

		type += meta::ForEachAttribute{
			[](Entity & e, auto && func) {
				auto & comp = e.get<Comp>();
				putils::reflection::for_each_attribute(comp,
					[&](const char * name, auto && member) {
						const auto typeIndex = putils::meta::type<putils_typeof(member)>::index;
						func(name, &member, typeIndex);
					}
				);
			}
		};
	}

	template<typename ... Comps>
	void registerComponentAttributeIterators(EntityManager & em) {
		putils::for_each_type<Comps...>([&](auto type) {
			using Type = putils_wrapped_type(type);
			registerComponentAttributeIterator<Type>(em);
		});
	}
}