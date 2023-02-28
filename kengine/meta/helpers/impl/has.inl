#include "has.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/core/log/helpers/kengine_log.hpp"

namespace kengine::meta {
	template<typename T>
	bool meta_component_implementation<has, T>::function(entt::const_handle e) noexcept {
		kengine_logf(*e.registry(), very_verbose, "meta::has", "Testing if [%u] has a %s", e.entity(), putils::reflection::get_class_name<T>());
		return e.all_of<T>();
	}
}