
#include "registerTypes.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::types{
	void registerTypes(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init", "Registering types");

		extern void registerkengineAdjustableComponent(entt::registry &) noexcept;
		registerkengineAdjustableComponent(r);

		extern void registerkengineAnimationComponent(entt::registry &) noexcept;
		registerkengineAnimationComponent(r);

		extern void registerkengineAnimationFilesComponent(entt::registry &) noexcept;
		registerkengineAnimationFilesComponent(r);

		extern void registerkengineModelAnimationComponent(entt::registry &) noexcept;
		registerkengineModelAnimationComponent(r);

		extern void registerkenginefunctionsAppearsInViewport(entt::registry &) noexcept;
		registerkenginefunctionsAppearsInViewport(r);

		extern void registerkengineCameraComponent(entt::registry &) noexcept;
		registerkengineCameraComponent(r);

		extern void registerkengineCommandLineComponent(entt::registry &) noexcept;
		registerkengineCommandLineComponent(r);

		extern void registerkengineViewportComponent(entt::registry &) noexcept;
		registerkengineViewportComponent(r);

		extern void registerkengineWindowComponent(entt::registry &) noexcept;
		registerkengineWindowComponent(r);

		extern void registerkengineDebugGraphicsComponent(entt::registry &) noexcept;
		registerkengineDebugGraphicsComponent(r);

		extern void registerkenginefunctionsExecute(entt::registry &) noexcept;
		registerkenginefunctionsExecute(r);

		extern void registerkengineGodRaysComponent(entt::registry &) noexcept;
		registerkengineGodRaysComponent(r);

		extern void registerkengineGraphicsComponent(entt::registry &) noexcept;
		registerkengineGraphicsComponent(r);

		extern void registerkengineHighlightComponent(entt::registry &) noexcept;
		registerkengineHighlightComponent(r);

		extern void registerkengineImGuiToolComponent(entt::registry &) noexcept;
		registerkengineImGuiToolComponent(r);

		extern void registerkengineInputComponent(entt::registry &) noexcept;
		registerkengineInputComponent(r);

		extern void registerkengineInstanceComponent(entt::registry &) noexcept;
		registerkengineInstanceComponent(r);

		extern void registerkengineDirLightComponent(entt::registry &) noexcept;
		registerkengineDirLightComponent(r);

		extern void registerkenginePointLightComponent(entt::registry &) noexcept;
		registerkenginePointLightComponent(r);

		extern void registerkengineSpotLightComponent(entt::registry &) noexcept;
		registerkengineSpotLightComponent(r);

		extern void registerkengineLuaComponent(entt::registry &) noexcept;
		registerkengineLuaComponent(r);

		extern void registerkengineLuaTableComponent(entt::registry &) noexcept;
		registerkengineLuaTableComponent(r);

		extern void registerkengineModelColliderComponent(entt::registry &) noexcept;
		registerkengineModelColliderComponent(r);

		extern void registerkengineModelComponent(entt::registry &) noexcept;
		registerkengineModelComponent(r);

		extern void registerkengineNameComponent(entt::registry &) noexcept;
		registerkengineNameComponent(r);

		extern void registerkengineNavMeshComponent(entt::registry &) noexcept;
		registerkengineNavMeshComponent(r);

		extern void registerkenginefunctionsOnClick(entt::registry &) noexcept;
		registerkenginefunctionsOnClick(r);

		extern void registerkenginePathfindingComponent(entt::registry &) noexcept;
		registerkenginePathfindingComponent(r);

		extern void registerkenginePhysicsComponent(entt::registry &) noexcept;
		registerkenginePhysicsComponent(r);

		extern void registerkenginePythonComponent(entt::registry &) noexcept;
		registerkenginePythonComponent(r);

		extern void registerkengineSelectedComponent(entt::registry &) noexcept;
		registerkengineSelectedComponent(r);

		extern void registerkengineModelSkeletonComponent(entt::registry &) noexcept;
		registerkengineModelSkeletonComponent(r);

		extern void registerkengineSkyBoxComponent(entt::registry &) noexcept;
		registerkengineSkyBoxComponent(r);

		extern void registerkengineSpriteComponent2D(entt::registry &) noexcept;
		registerkengineSpriteComponent2D(r);

		extern void registerkengineSpriteComponent3D(entt::registry &) noexcept;
		registerkengineSpriteComponent3D(r);

		extern void registerkengineTextComponent2D(entt::registry &) noexcept;
		registerkengineTextComponent2D(r);

		extern void registerkengineTextComponent3D(entt::registry &) noexcept;
		registerkengineTextComponent3D(r);

		extern void registerkengineTimeModulatorComponent(entt::registry &) noexcept;
		registerkengineTimeModulatorComponent(r);

		extern void registerkengineTransformComponent(entt::registry &) noexcept;
		registerkengineTransformComponent(r);

		extern void registerkenginemetaAttachTo(entt::registry &) noexcept;
		registerkenginemetaAttachTo(r);

		extern void registerkenginemetaAttributes(entt::registry &) noexcept;
		registerkenginemetaAttributes(r);

		extern void registerkenginemetaCopy(entt::registry &) noexcept;
		registerkenginemetaCopy(r);

		extern void registerkenginemetaCount(entt::registry &) noexcept;
		registerkenginemetaCount(r);

		extern void registerkenginemetaDetachFrom(entt::registry &) noexcept;
		registerkenginemetaDetachFrom(r);

		extern void registerkenginemetaDisplayImGui(entt::registry &) noexcept;
		registerkenginemetaDisplayImGui(r);

		extern void registerkenginemetaEditImGui(entt::registry &) noexcept;
		registerkenginemetaEditImGui(r);

		extern void registerkenginemetaForEachEntity(entt::registry &) noexcept;
		registerkenginemetaForEachEntity(r);

		extern void registerkenginemetaForEachEntityWithout(entt::registry &) noexcept;
		registerkenginemetaForEachEntityWithout(r);

		extern void registerkenginemetaGet(entt::registry &) noexcept;
		registerkenginemetaGet(r);

		extern void registerkenginemetaHas(entt::registry &) noexcept;
		registerkenginemetaHas(r);

		extern void registerkenginemetaLoadFromJSON(entt::registry &) noexcept;
		registerkenginemetaLoadFromJSON(r);

		extern void registerkenginemetaMatchString(entt::registry &) noexcept;
		registerkenginemetaMatchString(r);

		extern void registerkenginemetaSaveToJSON(entt::registry &) noexcept;
		registerkenginemetaSaveToJSON(r);

		extern void registerkenginemetaSize(entt::registry &) noexcept;
		registerkenginemetaSize(r);

		extern void registerputilsPoint2f(entt::registry &) noexcept;
		registerputilsPoint2f(r);

	}
}
