#include "remove.hpp"

// entt
#include <entt/entity/handle.hpp>

namespace kengine::meta {
	template<typename T>
	void meta_component_implementation<remove, T>::function(entt::handle e) noexcept {
		kengine_logf(*e.registry(), very_verbose, "meta::remove", "Removing [%u]'s %s", e.entity(), putils::reflection::get_class_name<T>());
		e.remove<T>();
	}
}
