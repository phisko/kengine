#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ... Comps>
	void register_save_to_json(entt::registry & r) noexcept;
}

#include "register_save_to_json.inl"
