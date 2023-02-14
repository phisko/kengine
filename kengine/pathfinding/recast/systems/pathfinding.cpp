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

// kengine data
#include "kengine/data/pathfinding.hpp"
#include "kengine/data/physics.hpp"
#include "kengine/data/transform.hpp"
#include "kengine/data/nav_mesh.hpp"

// kengine helpers
#include "kengine/helpers/instance_helper.hpp"
#include "kengine/helpers/matrix_helper.hpp"
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

// impl
#include "common.hpp"
#include "recast_agent.hpp"
#include "recast_crowd.hpp"
#include "recast_nav_mesh.hpp"

namespace kengine::systems::recast_impl {
	struct do_pathfinding {
		static void run(entt::registry & r, float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "recast", "Doing pathfinding");

			remove_old_agents(r);
			move_changed_agents(r);
			create_new_agents(r);
			update_crowds(r, delta_time);
		}

		static void remove_old_agents(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "recast", "Removing old agents");

			for (auto [e, agent] : r.view<data::recast_agent>(entt::exclude<data::pathfinding>).each()) {
				kengine_logf(r, verbose, "recast", "Removing agent [%u] from crowd [%u]", e, agent.crowd);
				auto & crowd = r.get<data::recast_crowd>(agent.crowd);
				crowd.crowd->removeAgent(agent.index);
				r.remove<data::recast_agent>(e);
			}
		}

		static void create_new_agents(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "recast", "Creating new agents");

			for (auto [e, pathfinding, transform] : r.view<data::pathfinding, data::transform>(entt::exclude<data::recast_agent>).each()) {
				if (pathfinding.environment == entt::null) {
					kengine_logf(r, very_verbose, "recast", "Entity [%u] has null environment", e);
					continue;
				}

				const auto crowd = get_crowd_component({ r, pathfinding.environment });
				if (!crowd) {
					kengine_logf(r, warning, "recast", "Entity [%u]'s environment [%u] has no crowd component", e, pathfinding.environment);
					continue;
				}

				kengine_logf(r, verbose, "recast", "Adding agent [%u] to crowd [%u]", e, pathfinding.environment);

				const auto object_info = get_object_info(get_environment_info({ r, pathfinding.environment }), transform, pathfinding);
				attach_agent_component({ r, e }, object_info, *crowd, pathfinding.environment);
			}
		}

		struct environment_info {
			putils::vec3f environment_scale;
			glm::mat4 model_to_world;
			glm::mat4 world_to_model;
		};
		static environment_info get_environment_info(entt::handle environment) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(*environment.registry(), very_verbose, "recast", "Getting environment info for [%u]", environment.entity());

			environment_info ret;

			const auto model_transform = instance_helper::try_get_model<data::transform>(environment);
			const auto & environment_transform = environment.get<data::transform>();

			ret.environment_scale = environment_transform.bounding_box.size;
			if (model_transform)
				ret.environment_scale *= model_transform->bounding_box.size;
			ret.model_to_world = matrix_helper::get_model_matrix(environment_transform, model_transform);
			ret.world_to_model = glm::inverse(ret.model_to_world);
			return ret;
		}

		struct object_info {
			putils::rect3f object_in_nav_mesh;
			float max_speed;
		};
		static object_info get_object_info(const environment_info & environment, const data::transform & transform, const data::pathfinding & pathfinding) noexcept {
			KENGINE_PROFILING_SCOPE;

			object_info ret;
			ret.object_in_nav_mesh = {
				matrix_helper::convert_to_referencial(transform.bounding_box.position, environment.world_to_model),
				transform.bounding_box.size / environment.environment_scale
			};
			ret.max_speed = putils::get_length(putils::point3f{ pathfinding.max_speed, 0.f, 0.f } / environment.environment_scale);
			return ret;
		}

		static data::recast_crowd * get_crowd_component(entt::handle e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(*e.registry(), very_verbose, "recast", "Getting crowd component for [%u]", e.entity());

			auto crowd = e.try_get<data::recast_crowd>();
			if (crowd)
				return crowd;

			kengine_logf(*e.registry(), very_verbose, "recast", "No crowd component in [%u], creating a new one", e.entity());
			const auto nav_mesh = instance_helper::try_get_model<data::recast_nav_mesh>(e);
			if (!nav_mesh) {
				kengine_logf(*e.registry(), very_verbose, "recast", "No recast nav mesh in [%u], cannot create crowd", e.entity());
				return crowd;
			}

			crowd = &e.emplace<data::recast_crowd>();
			crowd->crowd.reset(dtAllocCrowd());
			crowd->crowd->init(KENGINE_RECAST_MAX_AGENTS, nav_mesh->nav_mesh->getParams()->tileWidth, nav_mesh->nav_mesh.get());

			return crowd;
		}

		static void attach_agent_component(entt::handle e, const object_info & object_info, const data::recast_crowd & crowd, entt::entity crowd_id) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(*e.registry(), very_verbose, "recast", "Attaching agent component to [%u] (crowd [%u])", e.entity(), crowd_id);

			dtCrowdAgentParams params;
			fill_crowd_agent_params(params, object_info);

			params.separationWeight = 0.f;
			params.updateFlags = ~0; // All flags seem to be optimizations, enable them

			params.obstacleAvoidanceType = 0; // Default params, might want to change?
			params.queryFilterType = 0; // Default query type, might want to change?

			params.userData = (void *)e.entity();

			const auto idx = crowd.crowd->addAgent(object_info.object_in_nav_mesh.position.raw, &params);
			kengine_assert(*e.registry(), idx >= 0);

			auto & agent = e.emplace<data::recast_agent>();
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
			kengine_log(r, very_verbose, "recast", "Moving changed agents");

			for (auto [e, pathfinding, agent] : r.view<data::pathfinding, data::recast_agent>().each()) {
				if (pathfinding.environment == agent.crowd) {
					kengine_logf(r, very_verbose, "recast", "Entity [%u] has a null environment", e);
					continue;
				}

				const auto new_crowd = get_crowd_component({ r, pathfinding.environment });
				if (!new_crowd) {
					kengine_logf(r, warning, "recast", "Entity [%u]'s environment [%u] has no crowd component", e, pathfinding.environment);
					continue;
				}

				const auto old_crowd = r.try_get<data::recast_crowd>(agent.crowd);
				if (old_crowd) {
					kengine_logf(r, verbose, "recast", "Removing [%u] from its old crowd ([%u])", e, agent.crowd);
					old_crowd->crowd->removeAgent(agent.index);
				}

				const auto object_info = get_object_info(get_environment_info({ r, pathfinding.environment }), r.get<data::transform>(e), pathfinding);
				attach_agent_component({ r, e }, object_info, *new_crowd, pathfinding.environment);
			}
		}

		static void update_crowds(entt::registry & r, float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "recast", "Updating crowds");

			const auto view = r.view<data::recast_crowd>();
			std::for_each(std::execution::par_unseq, putils_range(view), [&](entt::entity environment) noexcept {
				const putils::scoped_thread_name thread_name("Recast crowd updater");
				const auto & [crowd] = view.get(environment);
				update_crowd(delta_time, { r, environment }, crowd);
			});
		}

		static void update_crowd(float delta_time, entt::handle environment, const data::recast_crowd & crowd) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(*environment.registry(), very_verbose, "recast", "Updating crowd for [%u]", environment.entity());

			const auto & nav_mesh = instance_helper::get_model<data::recast_nav_mesh>(environment);
			const auto environment_info = get_environment_info(environment);

			static dtCrowdAgent * active_agents[KENGINE_RECAST_MAX_AGENTS];

			const auto nb_agents = crowd.crowd->getActiveAgents(active_agents, (int)putils::lengthof(active_agents));

			auto & r = *environment.registry();

			// Overwrite agent with user-updated components
			for (int i = 0; i < nb_agents; ++i) {
				const auto agent = active_agents[i];
				const auto e = entt::entity(intptr_t(agent->params.userData));
				const auto & [transform, pathfinding] = r.get<data::transform, data::pathfinding>(e);
				write_to_agent({ r, e }, transform, pathfinding, environment_info, nav_mesh, crowd);
			}

			crowd.crowd->update(delta_time, nullptr);

			// Update user components with agent info
			for (int i = 0; i < nb_agents; ++i) {
				const auto agent = active_agents[i];
				const auto e = entt::entity(intptr_t(agent->params.userData));
				const auto & [transform, physics] = r.get<data::transform, data::physics>(e);
				read_from_agent(transform, physics, *agent, environment_info);
			}
		}

		static void read_from_agent(data::transform & transform, data::physics & physics, const dtCrowdAgent & agent, const environment_info & environment_info) noexcept {
			KENGINE_PROFILING_SCOPE;

			physics.movement = environment_info.environment_scale * putils::point3f{ agent.vel };
			transform.bounding_box.position = matrix_helper::convert_to_referencial(agent.npos, environment_info.model_to_world);
		}

		static void write_to_agent(entt::handle e, const data::transform & transform, const data::pathfinding & pathfinding, const environment_info & environment_info, const data::recast_nav_mesh & nav_mesh, const data::recast_crowd & crowd) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(*e.registry(), very_verbose, "recast", "Writing to agent [%u]", e.entity());

			const auto object_info = get_object_info(environment_info, transform, pathfinding);
			update_agent_component(e, object_info, crowd);

			const auto destination_in_model = matrix_helper::convert_to_referencial(pathfinding.destination, environment_info.world_to_model);
			const auto search_extents = putils::point3f{ pathfinding.search_distance, pathfinding.search_distance, pathfinding.search_distance } / environment_info.environment_scale;
			update_destination(e, nav_mesh, crowd, destination_in_model, search_extents);
		}

		static void update_agent_component(entt::handle e, const object_info & object_info, const data::recast_crowd & crowd) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(*e.registry(), very_verbose, "recast", "Updating agent component for [%u]", e.entity());

			const auto & agent = e.get<data::recast_agent>();
			const auto editableAgent = crowd.crowd->getEditableAgent(agent.index);
			fill_crowd_agent_params(editableAgent->params, object_info);
			memcpy(editableAgent->npos, object_info.object_in_nav_mesh.position.raw, sizeof(float[3]));
			memcpy(editableAgent->nvel, e.get<data::physics>().movement.raw, sizeof(float[3]));
		}

		static void update_destination(entt::handle e, const data::recast_nav_mesh & nav_mesh, const data::recast_crowd & crowd, const putils::point3f & destination_in_model, const putils::point3f & search_extents) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_logf(*e.registry(), very_verbose, "recast", "Updating destination for [%u]", e.entity());

			static const dtQueryFilter filter;
			dtPolyRef nearest_poly;
			float nearest_point[3];
			const auto status = nav_mesh.nav_mesh_query->findNearestPoly(destination_in_model.raw, search_extents.raw, &filter, &nearest_poly, nearest_point);
			if (dtStatusFailed(status) || nearest_poly == 0) {
				kengine_log(*e.registry(), very_verbose, "recast", "Failed to find nearest poly to destination");
				return;
			}

			const auto & agent = e.get<data::recast_agent>();
			if (!crowd.crowd->requestMoveTarget(agent.index, nearest_poly, nearest_point))
				kengine_assert_failed(*e.registry(), "[Recast] Failed to request move");
		}
	};

	void do_pathfinding(entt::registry & r, float delta_time) noexcept {
		do_pathfinding::run(r, delta_time);
	}
}