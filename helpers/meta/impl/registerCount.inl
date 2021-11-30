#include "registerCount.hpp"

#include "meta/Count.hpp"
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
