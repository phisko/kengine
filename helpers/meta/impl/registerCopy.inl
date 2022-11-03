#include "registerCopy.hpp"

// kengine meta
#include "meta/Copy.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerCopy() noexcept {
		KENGINE_PROFILING_SCOPE;
		registerMetaComponentImplementationWithPredicate<meta::Copy, std::is_copy_constructible, Comps...>(
			[](const auto t, const Entity & src, Entity & dst) noexcept {
				using T = putils_wrapped_type(t);
                dst += src.get<T>();
			}
		);
	}
}

