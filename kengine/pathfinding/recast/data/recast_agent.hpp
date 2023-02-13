#pragma once

// entt
#include <entt/entity/handle.hpp>

// putils
#include "putils/default_constructors.hpp"

// kengine core
#include "kengine/core/helpers/log_helper.hpp"

// impl
#include "recast_crowd.hpp"

namespace kengine::data {
	//! putils reflect all
	struct recast_agent {
		int index = 0;
		entt::handle crowd;

		~recast_agent() noexcept {
			if (!crowd)
				return;

			kengine_logf(*crowd.registry(), verbose, "recast", "Removing agent from crowd [%u]", crowd.entity());
			const auto & crowd_component = crowd.get<data::recast_crowd>();
			crowd_component.crowd->removeAgent(index);
		}

		PUTILS_DELETE_COPY(recast_agent);
		recast_agent() noexcept = default;
		recast_agent(recast_agent && rhs) noexcept {
			*this = std::move(rhs);
		}
		recast_agent & operator=(recast_agent && rhs) noexcept {
			std::swap(index, rhs.index);
			std::swap(crowd, rhs.crowd);
			return *this;
		}
	};
}

#include "recast_agent.rpp"