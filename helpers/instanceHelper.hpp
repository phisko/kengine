#pragma once

#include "kengine.hpp"
#include "data/InstanceComponent.hpp"
#include "assertHelper.hpp"

namespace kengine::instanceHelper {
	template<typename Comp>
	bool modelHas(const Entity & instance) noexcept;
	template<typename Comp>
	bool modelHas(const InstanceComponent & instance) noexcept;

	template<typename Comp>
	const Comp & getModel(const Entity & instance) noexcept;
	template<typename Comp>
	const Comp & getModel(const InstanceComponent & instance) noexcept;

	template<typename Comp>
	const Comp & tryGetModel(const Entity & instance) noexcept;
	template<typename Comp>
	const Comp & tryGetModel(const InstanceComponent & instance) noexcept;
}

#include "instanceHelper.inl"