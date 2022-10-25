#include "registerCopy.hpp"

#include "meta/Copy.hpp"
#include "helpers/registerMetaComponentImplementation.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerCopy() noexcept {
		registerMetaComponentImplementationWithPredicate<meta::Copy, std::is_copy_constructible, Comps...>(
			[](const auto t, const Entity & src, Entity & dst) noexcept {
				using T = putils_wrapped_type(t);
                dst += src.get<T>();
			}
		);
	}
}

