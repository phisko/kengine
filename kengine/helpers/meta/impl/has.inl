#include "has.hpp"

// entt
#include <entt/entity/handle.hpp>

namespace kengine {
	template<typename T>
	bool meta_component_implementation<meta::has, T>::function(entt::const_handle e) noexcept {
		return e.all_of<T>();
	}
}