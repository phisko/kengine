#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine model_instance
#include "kengine/model_instance/data/instance.hpp"

namespace kengine::instance_helper {
	template<typename Comp>
	bool model_has(entt::const_handle instance) noexcept;
	template<typename Comp>
	bool model_has(const entt::registry & r, const data::instance & instance) noexcept;

	template<typename Comp>
	const Comp & get_model(entt::const_handle instance) noexcept;
	template<typename Comp>
	const Comp & get_model(const entt::registry & r, const data::instance & instance) noexcept;

	template<typename Comp>
	const Comp * try_get_model(entt::const_handle instance) noexcept;
	template<typename Comp>
	const Comp * try_get_model(const entt::registry & r, const data::instance & instance) noexcept;
}

#include "instance_helper.inl"