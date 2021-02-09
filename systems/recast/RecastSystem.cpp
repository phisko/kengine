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

		void buildNavMeshes() noexcept;
		void doPathfinding(float deltaTime) noexcept;
	}

	EntityCreator * RecastSystem() noexcept {
		struct impl {
			static void init(Entity & e) noexcept {
				e += functions::OnEntityRemoved{ onEntityRemoved };
				e += functions::Execute{ execute };

				e += SystemSpecificShaderComponent<putils::gl::Program>{ std::make_unique<RecastDebugShader>() };
				e += GBufferShaderComponent{};

				e += AdjustableComponent{
					"Recast", {
						{ "Path optimization range", &recast::g_adjustables.pathOptimizationRange },
						{ "Editor mode (rebuild navmesh each frame)", &recast::g_adjustables.editorMode }
					}
				};
			}

			static void onEntityRemoved(Entity & e) noexcept {
				const auto agent = e.tryGet<RecastAgentComponent>();
				if (agent) {
					auto environment = entities[agent->crowd];
					environment.get<RecastCrowdComponent>().crowd->removeAgent(agent->index);
				}

				// It doesn't cost us anything to have floating RecastAgentComponents, so we don't remove them when the RecastCrowdComponent is removed
			}

			static void execute(float deltaTime) noexcept {
				recast::buildNavMeshes();
				recast::doPathfinding(deltaTime);
			}
		};

		return [](Entity & e) noexcept {
			impl::init(e);
		};
	}
}