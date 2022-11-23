#include "registerCopy.hpp"

// entt
#include <entt/entity/handle.hpp>

// kengine meta
#include "meta/Copy.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerCopy(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		registerMetaComponentImplementationWithPredicate<meta::Copy, std::is_copy_constructible, Comps...>(
			r, [](const auto t, entt::const_handle src, entt::handle dst) noexcept {
				using T = putils_wrapped_type(t);
				if constexpr (!std::is_empty<T>()) {
					const auto & srcComponent = src.get<T>();
					dst.emplace_or_replace<T>(srcComponent);
				}
				else
					dst.emplace<T>();
			}
		);
	}
}

