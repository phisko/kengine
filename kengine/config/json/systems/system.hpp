#pragma once

// entt
#include <entt/entity/fwd.hpp>

// kengine
#include "kengine/system_creator/helpers/system_creator_helper.hpp"

namespace kengine::config::json {
	DECLARE_KENGINE_SYSTEM_CREATOR(KENGINE_CONFIG_JSON_EXPORT, system)
}