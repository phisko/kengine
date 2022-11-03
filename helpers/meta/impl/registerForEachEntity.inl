#include "registerEditImGui.hpp"

// kengine meta
#include "meta/ForEachEntity.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerForEachEntity() noexcept {
		registerMetaComponentImplementation<meta::ForEachEntity, Comps...>(
			[](const auto t, auto && func) noexcept {
				using Type = putils_wrapped_type(t);
				for (auto [e, comp] : entities.with<Type>())
					func(e);
			}
		);

		registerMetaComponentImplementation<meta::ForEachEntityWithout, Comps...>(
			[](const auto t, auto && func) noexcept {
				using Type = putils_wrapped_type(t);
				for (auto [e, notComp] : entities.with<no<Type>>())
					func(e);
			}
		);
	}
}

