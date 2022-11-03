#include "registerHas.hpp"

// kengine meta
#include "meta/Has.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerHas() noexcept {
		registerMetaComponentImplementation<meta::Has, Comps...>(
			[](const auto t, const Entity & e) noexcept {
				using T = putils_wrapped_type(t);
				return e.has<T>();
			}
		);
	}
}

