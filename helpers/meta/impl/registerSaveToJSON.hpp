#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine {
	template<typename ... Comps>
	void registerSaveToJSON(entt::registry & r) noexcept;
}

#include "registerSaveToJSON.inl"
