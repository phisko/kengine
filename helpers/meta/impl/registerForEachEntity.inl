#include "registerEditImGui.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine meta
#include "meta/ForEachEntity.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerForEachEntity(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		registerMetaComponentImplementation<meta::ForEachEntity, Comps...>(
			r, [&](const auto t, auto && func) noexcept {
				KENGINE_PROFILING_SCOPE;

				using Type = putils_wrapped_type(t);
				for (const auto e : r.view<Type>())
					func({ r, e });
			}
		);

		registerMetaComponentImplementation<meta::ForEachEntityWithout, Comps...>(
			r, [&](const auto t, auto && func) noexcept {
				KENGINE_PROFILING_SCOPE;

				using Type = putils_wrapped_type(t);
				r.each([&](entt::entity e) {
					if (!r.all_of<Type>(e))
						func({ r, e });
				});
			}
		);
	}
}

