#include "registerDetachFrom.hpp"

// kengine meta
#include "meta/DetachFrom.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerDetachFrom() noexcept {
		registerMetaComponentImplementation<meta::DetachFrom, Comps...>(
			[](const auto t, Entity & e) noexcept {
				using Type = putils_wrapped_type(t);
				e.detach<Type>();
			}
		);
	}
}

