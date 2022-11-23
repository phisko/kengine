#include "registerGet.hpp"

// entt
#include <entt/entity/handle.hpp>

// kengine meta
#include "meta/Get.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerGet(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		registerMetaComponentImplementation<meta::Get, Comps...>(
			r, [&](const auto t, entt::handle e) noexcept -> void * {
				using T = putils_wrapped_type(t);
				if constexpr (!std::is_empty<T>())
					return &e.get<T>();
				else {
					static T instance;
					return &instance;
				}
			}
		);

		registerMetaComponentImplementation<meta::GetConst, Comps...>(
			r, [](const auto t, entt::const_handle e) noexcept -> const void * {
				using T = putils_wrapped_type(t);
				if constexpr (!std::is_empty<T>())
					return &e.get<T>();
				else {
					static const T instance;
					return &instance;
				}
			}
		);
	}
}