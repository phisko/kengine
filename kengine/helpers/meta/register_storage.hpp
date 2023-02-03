#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename... Comps>
	void register_storage(entt::registry & r) noexcept;
}

#include "register_storage.inl"
