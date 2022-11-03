#include "registerCount.hpp"

// kengine meta
#include "meta/Count.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerCount() noexcept {
		registerMetaComponentImplementation<meta::Count, Comps...>(
			[](const auto t) noexcept {
				using Type = putils_wrapped_type(t);
				return entities.with<Type>().size();
			}
		);
	}
}
