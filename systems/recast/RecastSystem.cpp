#include "RecastSystem.hpp"
#include "RecastDebugShader.hpp"
#include "Common.hpp"

#include "data/AdjustableComponent.hpp"
#include "data/ShaderComponent.hpp"

#include "RecastAgentComponent.hpp"
#include "RecastCrowdComponent.hpp"

#include "functions/Execute.hpp"
#include "functions/OnEntityRemoved.hpp"

namespace kengine {
	namespace recast {
		Adjustables g_adjustables;
		EntityManager * g_em;
	}
	using namespace recast;

#pragma region declarations
	static void onEntityRemoved(Entity & e);
	static void execute(float deltaTime);
#pragma endregion
	EntityCreator * RecastSystem(EntityManager & em) {
		g_em = &em;

		return [](Entity & e) {
			e += functions::OnEntityRemoved{ onEntityRemoved };
			e += functions::Execute{ execute };
			e += makeGBufferShaderComponent<RecastDebugShader>(*g_em);

			e += AdjustableComponent{
				"Recast", {
					{ "Path optimization range", &g_adjustables.pathOptimizationRange }
				}
			};
		};
	}

	static void onEntityRemoved(Entity & e) {
		if (e.has<RecastAgentComponent>()) {
			const auto & agent = e.get<RecastAgentComponent>();
			auto environment = g_em->getEntity(agent.crowd);
			environment.get<RecastCrowdComponent>().crowd->removeAgent(agent.index);
		}

		// It doesn't cost us anything to have floating RecastAgentComponents, so we don't remove them when the RecastCrowdComponent is removed
	}

#pragma region declarations
	namespace recast {
		void buildNavMeshes();
		void doPathfinding(float deltaTime);
	}
#pragma endregion
	static void execute(float deltaTime) {
		buildNavMeshes();
		doPathfinding(deltaTime);
	}
}