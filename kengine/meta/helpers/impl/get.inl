#include "get.hpp"

// entt
#include <entt/entity/handle.hpp>

namespace kengine {
	template<typename T>
	void * meta_component_implementation<meta::get, T>::function(entt::handle e) noexcept {
		kengine_logf(*e.registry(), very_verbose, "meta::get", "Getting [%u]'s %s", e.entity(), putils::reflection::get_class_name<T>());

		if constexpr (!std::is_empty<T>())
			return &e.get<T>();
		else {
			kengine_log(*e.registry(), very_verbose, "meta::get", "Component is empty, using static instance");
			static T instance;
			return &instance;
		}
	}

	template<typename T>
	const void * meta_component_implementation<meta::get_const, T>::function(entt::const_handle e) noexcept {
		kengine_logf(*e.registry(), very_verbose, "meta::get_const", "Getting [%u]'s %s as const", e.entity(), putils::reflection::get_class_name<T>());

		if constexpr (!std::is_empty<T>())
			return &e.get<T>();
		else {
			kengine_log(*e.registry(), very_verbose, "meta::get_const", "Component is empty, using static instance");
			static const T instance;
			return &instance;
		}
	}
}