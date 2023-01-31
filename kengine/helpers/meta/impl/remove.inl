#include "remove.hpp"

// entt
#include <entt/entity/handle.hpp>

namespace kengine {
	template<typename T>
	void meta_component_implementation<meta::remove, T>::function(entt::handle e) noexcept {
		e.remove<T>();
	}
}
