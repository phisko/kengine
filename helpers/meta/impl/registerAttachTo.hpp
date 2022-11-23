#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ... Comps>
	void registerAttachTo(entt::registry & r) noexcept;
}

#include "registerAttachTo.inl"