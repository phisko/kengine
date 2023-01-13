
#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::types {
	KENGINE_TYPE_REGISTRATION_EXPORT void add_type_registrator(entt::registry & r) noexcept;
}