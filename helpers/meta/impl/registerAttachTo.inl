#include "registerAttachTo.hpp"

// entt
#include <entt/entity/handle.hpp>

// kengine meta
#include "meta/AttachTo.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerAttachTo(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		registerMetaComponentImplementation<meta::AttachTo, Comps...>(
			r, [](const auto t, entt::handle e) noexcept {
				using Type = putils_wrapped_type(t);
				e.emplace_or_replace<Type>();
			}
		);
	}
}