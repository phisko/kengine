#include "create_all_systems.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine system_creator
#include "kengine/system_creator/functions/create_system.hpp"

namespace kengine {
	void create_all_systems(entt::registry & destination_registry, const entt::registry * main_registry) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(destination_registry, log, "create_all_systems", "Creating all pre-registered systems");

		if (!main_registry)
			main_registry = &destination_registry;

		for (const auto & [e, create_system] : main_registry->view<functions::create_system>().each()) {
			kengine_logf(destination_registry, verbose, "create_all_systems", "Creating system from [%u]", e);
			create_system(destination_registry);
		}
	}
}