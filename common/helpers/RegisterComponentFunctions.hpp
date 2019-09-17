#pragma once

#include "EntityManager.hpp"
#include "functions/Basic.hpp"

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
		em.registerComponentFunction<Comp>(functions::Has{ detail::has<Comp> });
		em.registerComponentFunction<Comp>(functions::Attach{ detail::attach<Comp> });
		em.registerComponentFunction<Comp>(functions::Detach{ detail::detach<Comp> });
	}

	template<typename ... Comps>
	void registerComponentsFunctions(kengine::EntityManager & em) {
		pmeta_for_each(Comps, [&](auto type) {
			using Type = pmeta_wrapped(type);
			registerComponentFunctions<Type>(em);
		});
	}
}
