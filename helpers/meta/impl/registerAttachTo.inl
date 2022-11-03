#include "registerAttachTo.hpp"

// kengine meta
#include "meta/AttachTo.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerAttachTo() noexcept {
		KENGINE_PROFILING_SCOPE;
		registerMetaComponentImplementation<meta::AttachTo, Comps...>(
			[](const auto t, Entity & e) noexcept {
				using Type = putils_wrapped_type(t);
				e.attach<Type>();
			}
		);
	}
}