#include "registerGet.hpp"

#include "meta/Get.hpp"
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

