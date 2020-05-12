#pragma once

#include "EntityManager.hpp"
#include "meta/ForEachEntity.hpp"
#include "helpers/TypeHelper.hpp"

namespace kengine {
	template<typename Comp>
	void registerComponentEntityIterator(EntityManager & em) {
		auto type = TypeHelper::getTypeEntity<Comp>(em);

		type += meta::ForEachEntity{
			[&](auto && func) {
				for (auto & [e, comp] : em.getEntities<Comp>())
					func(e);
			}
		};

		type += meta::ForEachEntityWithout{
			[&](auto && func) {
				for (auto & [e, notComp] : em.getEntities<no<Comp>>())
					func(e);
			}
		};
	}

	template<typename ... Comps>
	void registerComponentEntityIterators(EntityManager & em) {
		putils::for_each_type<Comps...>([&](auto type) {
			using Type = putils_wrapped_type(type);
			registerComponentEntityIterator<Type>(em);
		});
	}
}