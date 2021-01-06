#include "registerComponentFunctions.hpp"

#include "kengine.hpp"
#include "meta/Has.hpp"
#include "meta/AttachTo.hpp"
#include "meta/DetachFrom.hpp"
#include "helpers/typeHelper.hpp"

namespace kengine {
	namespace impl {
		template<typename Comp>
		bool has(const Entity & e) noexcept { return e.has<Comp>(); }

		template<typename Comp>
		void attach(Entity & e) noexcept { e.attach<Comp>(); }

		template<typename Comp>
		void detach(Entity & e) noexcept { e.detach<Comp>(); }
	}

	template<typename ... Comps>
	void registerComponentFunctions() noexcept {
		putils::for_each_type<Comps...>([](auto t) noexcept {
			using Type = putils_wrapped_type(t);
			auto type = typeHelper::getTypeEntity<Type>();
			type += meta::Has{ impl::has<Type> };
			type += meta::AttachTo{ impl::attach<Type> };
			type += meta::DetachFrom{ impl::detach<Type> };
		});
	}
}
