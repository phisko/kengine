#include "RecastSystem.hpp"
#include "RecastDebugShader.hpp"
#include "Common.hpp"

#include "data/AdjustableComponent.hpp"
#include "data/ShaderComponent.hpp"

#include "RecastAgentComponent.hpp"
#include "RecastCrowdComponent.hpp"

#include "functions/Execute.hpp"
#include "functions/OnEntityRemoved.hpp"

namespace kengine::recast {
	Adjustables g_adjustables;
	EntityManager * g_em;

	void buildNavMeshes();
	void doPathfinding(float deltaTime);

	struct impl {
		static void init(Entity & e) {
			e += functions::OnEntityRemoved{ onEntityRemoved };
			e += functions::Execute{ execute };

			e += opengl::ShaderComponent{ std::make_unique<RecastDebugShader>(*g_em) };
			e += opengl::GBufferShaderComponent{};

			e += AdjustableComponent{
				"Recast", {
					{ "Path optimization range", &g_adjustables.pathOptimizationRange }
				}
			};
		}

		static void onEntityRemoved(Entity & e) {
			const auto agent = e.tryGet<RecastAgentComponent>();
			if (agent) {
				auto environment = g_em->getEntity(agent->crowd);
				environment.get<RecastCrowdComponent>().crowd->removeAgent(agent->index);
			}

			// It doesn't cost us anything to have floating RecastAgentComponents, so we don't remove them when the RecastCrowdComponent is removed
		}

		static void execute(float deltaTime) {
			buildNavMeshes();
			doPathfinding(deltaTime);
		}
	};
}

namespace kengine {
	EntityCreator * RecastSystem(EntityManager & em) {
		recast::g_em = &em;
		return [](Entity & e) {
			recast::impl::init(e);
		};
	}
}