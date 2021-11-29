#include "registerAttachTo.hpp"

#include "meta/AttachTo.hpp"
#include "helpers/registerMetaComponentImplementation.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerAttachTo() noexcept {
		registerMetaComponentImplementation<meta::AttachTo, Comps...>(
			[](const auto t, Entity & e) noexcept {
				using Type = putils_wrapped_type(t);
				e.attach<Type>();
			}
		);
	}
}