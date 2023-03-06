#ifndef KENGINE_RECAST_MAX_AGENTS
#define KENGINE_RECAST_MAX_AGENTS 1024
#endif

// stl
#include <algorithm>
#include <execution>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/lengthof.hpp"
#include "putils/range.hpp"
#include "putils/thread_name.hpp"

// kengine
#include "kengine/core/data/transform.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/glm/helpers/convert_to_referencial.hpp"
#include "kengine/glm/helpers/get_model_matrix.hpp"
#include "kengine/instance/helpers/get_model.hpp"
#include "kengine/instance/helpers/try_get_model.hpp"
#include "kengine/pathfinding/data/nav_mesh.hpp"
#include "kengine/pathfinding/data/pathfinding.hpp"
#include "kengine/pathfinding/recast/data/agent.hpp"
#include "kengine/pathfinding/recast/data/crowd.hpp"
#include "kengine/pathfinding/recast/data/nav_mesh.hpp"
#include "kengine/physics/data/physics.hpp"

#include "common.hpp"

namespace kengine::pathfinding::recast {
	struct do_pathfinding {
		static void run(entt::registry & r, float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Doing pathfinding");

			remove_old_agents(r);
			move_changed_agents(r);
			create_new_agents(r);
			update_crowds(r, delta_time);
		}

		static void remove_old_agents(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Removing old agents");

			for (auto [e, agent] : r.view<recast::agent>(entt::exclude<kengine::pathfinding::pathfinding>).each()) {
				kengine_logf(r, verbose, log_category, "Removing agent {} from crowd {}", e, agent.crowd);
				auto & crowd = r.get<recast::crowd>(agent.crowd);
				crowd.ptr->removeAgent(agent.index);
				r.remove<recast::agent>(e);
			}
		}

		static void create_new_agents(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Creating new agents");

			for (auto [e, pathfinding, transform] : r.view<kengine::pathfinding::pathfinding, core::transform>(entt::exclude<agent>).each()) {
				if (pathfinding.environment == entt::null) {
					kengine_logf(r, very_verbose, log_category, "Entity {} has null environment", e);
					continue;
				}

				const auto crowd = get_crowd_component({ r, pathfinding.environment });
				if (!crowd) {
					kengine_logf(r, warning, log_category, "Entity {}'s environment {} has no crowd component", e, pathfinding.environment);
					continue;
				}

				kengine_logf(r, verbose, log_category, "Adding agent {} to crowd {}", e, pathfinding.environment);

				const auto object_info = get_object_info(get_environment_info({ r, pathfinding.environment }), transform, pathfinding);
				attach_agent_component({ r, e }, object_info, *crowd, pathfinding.environment);
			}
		}

		struct environment_info {
			putils::vec3f environment_scale;
			::glm::mat4 model_to_world;
			::glm::mat4 world_to_model;
		};
		static environment_info get_environment_info(entt::handle environment) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(*environment.registry(), very_verbose, log_category, "Getting environment info for {}", environment);

			environment_info ret;

			const auto model_transform = instance::try_get_model<core::transform>(environment);
			const auto & environment_transform = environment.get<core::transform>();

			ret.environment_scale = environment_transform.bounding_box.size;
			if (model_transform)
				ret.environment_scale *= model_transform->bounding_box.size;
			ret.model_to_world = glm::get_model_matrix(environment_transform, model_transform);
			ret.world_to_model = ::glm::inverse(ret.model_to_world);
			return ret;
		}

		struct object_info {
			putils::rect3f object_in_nav_mesh;
			float max_speed;
		};
		static object_info get_object_info(const environment_info & environment, const core::transform & transform, const pathfinding & pathfinding) noexcept {
			KENGINE_PROFILING_SCOPE;

			object_info ret;
			ret.object_in_nav_mesh = {
				glm::convert_to_referencial(transform.bounding_box.position, environment.world_to_model),
				transform.bounding_box.size / environment.environment_scale
			};
			ret.max_speed = putils::get_length(putils::point3f{ pathfinding.max_speed, 0.f, 0.f } / environment.environment_scale);
			return ret;
		}

		static crowd * get_crowd_component(entt::handle e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(*e.registry(), very_verbose, log_category, "Getting crowd component for {}", e);

			auto crowd = e.try_get<recast::crowd>();
			if (crowd)
				return crowd;

			kengine_logf(*e.registry(), very_verbose, log_category, "No crowd component in {}, creating a new one", e);
			const auto nav_mesh = instance::try_get_model<recast::nav_mesh>(e);
			if (!nav_mesh) {
				kengine_logf(*e.registry(), very_verbose, log_category, "No recast nav mesh in {}, cannot create crowd", e);
				return crowd;
			}

			crowd = &e.emplace<recast::crowd>();
			crowd->ptr.reset(dtAllocCrowd());
			crowd->ptr->init(KENGINE_RECAST_MAX_AGENTS, nav_mesh->ptr->getParams()->tileWidth, nav_mesh->ptr.get());

			return crowd;
		}

		static void attach_agent_component(entt::handle e, const object_info & object_info, const crowd & crowd, entt::entity crowd_id) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(*e.registry(), very_verbose, log_category, "Attaching agent component to {} (crowd {})", e, crowd_id);

			dtCrowdAgentParams params;
			fill_crowd_agent_params(params, object_info);

			params.separationWeight = 0.f;
			params.updateFlags = ~0; // All flags seem to be optimizations, enable them

			params.obstacleAvoidanceType = 0; // Default params, might want to change?
			params.queryFilterType = 0; // Default query type, might want to change?

			params.userData = (void *)e.entity();

			const auto idx = crowd.ptr->addAgent(object_info.object_in_nav_mesh.position.raw, &params);
			kengine_assert(*e.registry(), idx >= 0);

			auto & agent = e.emplace<recast::agent>();
			agent.index = idx;
			agent.crowd = { *e.registry(), crowd_id };
		}

		static void fill_crowd_agent_params(dtCrowdAgentParams & params, const object_info & object_info) noexcept {
			KENGINE_PROFILING_SCOPE;

			params.radius = std::max(object_info.object_in_nav_mesh.size.x, object_info.object_in_nav_mesh.size.z);
			params.height = object_info.object_in_nav_mesh.size.y;
			params.maxAcceleration = object_info.max_speed;
			params.maxSpeed = params.maxAcceleration;

			params.collisionQueryRange = params.radius * 2.f;
			params.pathOptimizationRange = params.collisionQueryRange * g_adjustables.path_optimization_range;
		}

		static void move_changed_agents(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Moving changed agents");

			for (auto [e, pathfinding, agent] : r.view<kengine::pathfinding::pathfinding, recast::agent>().each()) {
				if (pathfinding.environment == agent.crowd) {
					kengine_logf(r, very_verbose, log_category, "Entity {} has a null environment", e);
					continue;
				}

				const auto new_crowd = get_crowd_component({ r, pathfinding.environment });
				if (!new_crowd) {
					kengine_logf(r, warning, log_category, "Entity {}'s environment {} has no crowd component", e, pathfinding.environment);
					continue;
				}

				const auto old_crowd = r.try_get<crowd>(agent.crowd);
				if (old_crowd) {
					kengine_logf(r, verbose, log_category, "Removing {} from its old crowd ({})", e, agent.crowd);
					old_crowd->ptr->removeAgent(agent.index);
				}

				const auto object_info = get_object_info(get_environment_info({ r, pathfinding.environment }), r.get<core::transform>(e), pathfinding);
				attach_agent_component({ r, e }, object_info, *new_crowd, pathfinding.environment);
			}
		}

		static void update_crowds(entt::registry & r, float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Updating crowds");

			const auto view = r.view<crowd>();
			std::for_each(std::execution::par_unseq, putils_range(view), [&](entt::entity environment) noexcept {
				const putils::scoped_thread_name thread_name("Recast crowd updater");
				const auto & [crowd] = view.get(environment);
				update_crowd(delta_time, { r, environment }, crowd);
			});
		}

		static void update_crowd(float delta_time, entt::handle environment, const crowd & crowd) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(*environment.registry(), very_verbose, log_category, "Updating crowd for {}", environment);

			const auto & nav_mesh = instance::get_model<recast::nav_mesh>(environment);
			const auto environment_info = get_environment_info(environment);

			static dtCrowdAgent * active_agents[KENGINE_RECAST_MAX_AGENTS];

			const auto nb_agents = crowd.ptr->getActiveAgents(active_agents, (int)putils::lengthof(active_agents));

			auto & r = *environment.registry();

			// Overwrite agent with user-updated components
			for (int i = 0; i < nb_agents; ++i) {
				const auto agent = active_agents[i];
				const auto e = entt::entity(intptr_t(agent->params.userData));
				const auto & [transform, pathfinding] = r.get<core::transform, kengine::pathfinding::pathfinding>(e);
				write_to_agent({ r, e }, transform, pathfinding, environment_info, nav_mesh, crowd);
			}

			crowd.ptr->update(delta_time, nullptr);

			// Update user components with agent info
			for (int i = 0; i < nb_agents; ++i) {
				const auto agent = active_agents[i];
				const auto e = entt::entity(intptr_t(agent->params.userData));
				const auto & [transform, physics] = r.get<core::transform, physics::physics>(e);
				read_from_agent(transform, physics, *agent, environment_info);
			}
		}

		static void read_from_agent(core::transform & transform, physics::physics & physics, const dtCrowdAgent & agent, const environment_info & environment_info) noexcept {
			KENGINE_PROFILING_SCOPE;

			physics.movement = environment_info.environment_scale * putils::point3f{ agent.vel };
			transform.bounding_box.position = glm::convert_to_referencial(agent.npos, environment_info.model_to_world);
		}

		static void write_to_agent(entt::handle e, const core::transform & transform, const kengine::pathfinding::pathfinding & pathfinding, const environment_info & environment_info, const nav_mesh & nav_mesh, const crowd & crowd) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(*e.registry(), very_verbose, log_category, "Writing to agent {}", e);

			const auto object_info = get_object_info(environment_info, transform, pathfinding);
			update_agent_component(e, object_info, crowd);

			const auto destination_in_model = glm::convert_to_referencial(pathfinding.destination, environment_info.world_to_model);
			const auto search_extents = putils::point3f{ pathfinding.search_distance, pathfinding.search_distance, pathfinding.search_distance } / environment_info.environment_scale;
			update_destination(e, nav_mesh, crowd, destination_in_model, search_extents);
		}

		static void update_agent_component(entt::handle e, const object_info & object_info, const crowd & crowd) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(*e.registry(), very_verbose, log_category, "Updating agent component for {}", e);

			const auto & agent = e.get<recast::agent>();
			const auto editableAgent = crowd.ptr->getEditableAgent(agent.index);
			fill_crowd_agent_params(editableAgent->params, object_info);
			memcpy(editableAgent->npos, object_info.object_in_nav_mesh.position.raw, sizeof(float[3]));
			memcpy(editableAgent->nvel, e.get<physics::physics>().movement.raw, sizeof(float[3]));
		}

		static void update_destination(entt::handle e, const nav_mesh & nav_mesh, const crowd & crowd, const putils::point3f & destination_in_model, const putils::point3f & search_extents) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(*e.registry(), very_verbose, log_category, "Updating destination for {}", e);

			static const dtQueryFilter filter;
			dtPolyRef nearest_poly;
			float nearest_point[3];
			const auto status = nav_mesh.nav_mesh_query->findNearestPoly(destination_in_model.raw, search_extents.raw, &filter, &nearest_poly, nearest_point);
			if (dtStatusFailed(status) || nearest_poly == 0) {
				kengine_log(*e.registry(), very_verbose, log_category, "Failed to find nearest poly to destination");
				return;
			}

			const auto & agent = e.get<recast::agent>();
			if (!crowd.ptr->requestMoveTarget(agent.index, nearest_poly, nearest_point))
				kengine_assert_failed(*e.registry(), "[Recast] Failed to request move");
		}
	};

	void do_pathfinding(entt::registry & r, float delta_time) noexcept {
		do_pathfinding::run(r, delta_time);
	}
}