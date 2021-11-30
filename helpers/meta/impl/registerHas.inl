#include "registerHas.hpp"

#include "meta/Has.hpp"
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

