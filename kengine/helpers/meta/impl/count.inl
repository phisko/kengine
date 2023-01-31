#include "count.hpp"

// entt
#include <entt/entity/registry.hpp>

namespace kengine {
	template<typename T>
	size_t meta_component_implementation<meta::count, T>::function(entt::registry & r) noexcept {
		const auto view = r.view<T>();
		if constexpr (requires { view.size(); }) // in-place-delete views don't have size()
			return view.size();
		else
			return view.size_hint();
	}
}
