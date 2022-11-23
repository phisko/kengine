#include "registerDetachFrom.hpp"

// entt
#include <entt/entity/handle.hpp>

// kengine meta
#include "meta/DetachFrom.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerDetachFrom(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		registerMetaComponentImplementation<meta::DetachFrom, Comps...>(
			r, [](const auto t, entt::handle e) noexcept {
				using Type = putils_wrapped_type(t);
				e.remove<Type>();
			}
		);
	}
}

