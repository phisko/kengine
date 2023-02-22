#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::meta {
	template<typename... Comps>
	void register_storage(entt::registry & r) noexcept;

	template<typename... Comps>
	bool is_storage_registered(const entt::registry & r) noexcept;
}

#include "register_storage.inl"
