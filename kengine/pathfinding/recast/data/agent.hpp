#pragma once

// entt
#include <entt/entity/handle.hpp>

// putils
#include "putils/default_constructors.hpp"

// kengine
#include "kengine/core/log/helpers/kengine_log.hpp"

// impl
#include "crowd.hpp"

namespace kengine::pathfinding::recast {
	//! putils reflect all
	//! class_name: recast_agent
	struct agent {
		int index = 0;
		entt::handle crowd;

		~agent() noexcept {
			if (!crowd)
				return;

			kengine_logf(*crowd.registry(), verbose, "recast", "Removing agent from crowd {}", crowd);
			const auto & crowd_component = crowd.get<recast::crowd>();
			crowd_component.ptr->removeAgent(index);
		}

		PUTILS_DELETE_COPY(agent);
		agent() noexcept = default;
		agent(agent && rhs) noexcept {
			*this = std::move(rhs);
		}
		agent & operator=(agent && rhs) noexcept {
			std::swap(index, rhs.index);
			std::swap(crowd, rhs.crowd);
			return *this;
		}
	};
}

#include "agent.rpp"