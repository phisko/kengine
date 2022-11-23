#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine data
#include "data/InstanceComponent.hpp"

namespace kengine::instanceHelper {
	template<typename Comp>
	bool modelHas(entt::const_handle instance) noexcept;
	template<typename Comp>
	bool modelHas(const entt::registry & r, const InstanceComponent & instance) noexcept;

	template<typename Comp>
	const Comp & getModel(entt::const_handle instance) noexcept;
	template<typename Comp>
	const Comp & getModel(const entt::registry & r, const InstanceComponent & instance) noexcept;

	template<typename Comp>
	const Comp * tryGetModel(entt::const_handle instance) noexcept;
	template<typename Comp>
	const Comp * tryGetModel(const entt::registry & r, const InstanceComponent & instance) noexcept;
}

#include "instanceHelper.inl"