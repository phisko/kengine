#pragma once

// glm
#include <glm/glm.hpp>

// putils
#include "putils/point.hpp"

// kengine
#include "kengine/pathfinding/data/nav_mesh.hpp"
#include "kengine/render/data/model_data.hpp"

#include "config.hpp"

namespace kengine::pathfinding::recast {
	static constexpr auto log_category = "pathfinding_recast";

	extern const config * g_config;
	void build_recast_component(entt::registry & r, entt::entity e, const render::model_data & model_data, const kengine::pathfinding::nav_mesh & nav_mesh) noexcept;
	void process_built_recast_components(entt::registry & r) noexcept;
	void do_pathfinding(entt::registry & r, float delta_time) noexcept;
}