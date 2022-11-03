#include "registerGet.hpp"

// kengine meta
#include "meta/Get.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerGet() noexcept {
		registerMetaComponentImplementation<meta::Get, Comps...>(
			[](const auto t, const Entity & e) noexcept {
				using T = putils_wrapped_type(t);
				return (void *)&e.get<T>();
			}
		);
	}
}

