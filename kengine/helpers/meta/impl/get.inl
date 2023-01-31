#include "get.hpp"

// entt
#include <entt/entity/handle.hpp>

namespace kengine {
	template<typename T>
	void * meta_component_implementation<meta::get, T>::function(entt::handle e) noexcept {
		if constexpr (!std::is_empty<T>())
			return &e.get<T>();
		else {
			static T instance;
			return &instance;
		}
	}

	template<typename T>
	const void * meta_component_implementation<meta::get_const, T>::function(entt::const_handle e) noexcept {
		if constexpr (!std::is_empty<T>())
			return &e.get<T>();
		else {
			static const T instance;
			return &instance;
		}
	}
}