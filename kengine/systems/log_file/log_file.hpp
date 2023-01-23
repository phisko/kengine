#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_LOG_FILE_EXPORT entt::entity add_log_file(entt::registry & r) noexcept;
}