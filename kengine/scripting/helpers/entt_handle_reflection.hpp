#pragma once

// entt
#include <entt/entity/handle.hpp>

// reflection
#include "putils/reflection.hpp"

// Reflection API for entt::handle
// We use entt::handle as entt::entity is a scalar and doesn't play well with scripting languages
#define refltype entt::handle
putils_reflection_info {
	putils_reflection_custom_class_name(entt_handle);
};
#undef refltype
