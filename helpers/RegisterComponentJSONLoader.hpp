#pragma once

#include "EntityManager.hpp"
#include "reflection/json_helper.hpp"
#include "meta/LoadFromJSON.hpp"
#include "helpers/TypeHelper.hpp"

namespace kengine {
	template<typename Comp>
	void registerComponentJSONLoader(kengine::EntityManager & em);

	template<typename ... Comps>
	void registerComponentJSONLoaders(kengine::EntityManager & em);
}

namespace kengine {
	namespace detail {
		template<typename Component>
		static void loadJSONComponent(const putils::json & jsonEntity, kengine::Entity & e) {
			const auto it = jsonEntity.find(putils::reflection::get_class_name<Component>());
			if (it == jsonEntity.end())
				return;
			auto & comp = e.attach<Component>();
			putils::reflection::fromJSON(*it, comp);
		}
	}

	template<typename Comp>
	void registerComponentJSONLoader(kengine::EntityManager & em) {
		auto type = TypeHelper::getTypeEntity<Comp>(em);
		type += meta::LoadFromJSON{ detail::loadJSONComponent<Comp> };
	}

	template<typename ... Comps>
	void registerComponentJSONLoaders(kengine::EntityManager & em) {
		putils::for_each_type<Comps...>([&](auto type) {
			using Type = putils_wrapped_type(type);
			registerComponentJSONLoader<Type>(em);
		});
	}
}