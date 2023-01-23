#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace kengine::systems {
	KENGINE_LOG_STDOUT_EXPORT entt::entity add_log_stdout(entt::registry & r) noexcept;
}