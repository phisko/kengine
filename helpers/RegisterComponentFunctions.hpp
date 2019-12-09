#pragma once

#include "EntityManager.hpp"
#include "meta/Has.hpp"
#include "meta/AttachTo.hpp"
#include "meta/DetachFrom.hpp"
#include "helpers/TypeHelper.hpp"

namespace kengine {
	template<typename Comp>
	void registerComponentFunctions(kengine::EntityManager & em);

	template<typename ... Comps>
	void registerComponentsFunctions(kengine::EntityManager & em);
}

namespace kengine {
	namespace detail {
		template<typename Comp>
		bool has(const kengine::Entity & e) { return e.has<Comp>(); }

		template<typename Comp>
		void attach(kengine::Entity & e) { e.attach<Comp>(); }

		template<typename Comp>
		void detach(kengine::Entity & e) { e.detach<Comp>(); }
	}

	template<typename Comp>
	void registerComponentFunctions(kengine::EntityManager & em) {
		auto type = TypeHelper::getTypeEntity<Comp>(em);
		type += meta::Has{ detail::has<Comp> };
		type += meta::AttachTo{ detail::attach<Comp> };
		type += meta::DetachFrom{ detail::detach<Comp> };
	}

	template<typename ... Comps>
	void registerComponentsFunctions(kengine::EntityManager & em) {
		putils::for_each_type<Comps...>([&](auto type) {
			using Type = putils_wrapped_type(type);
			registerComponentFunctions<Type>(em);
		});
	}
}
