#include "create_all_systems.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine functions
#include "kengine/functions/create_system.hpp"

// kengine helpers
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	void create_all_systems(entt::registry & destination_registry, const entt::registry * main_registry) noexcept {
		KENGINE_PROFILING_SCOPE;

		if (!main_registry)
			main_registry = &destination_registry;

		for (const auto & [e, create_system] : main_registry->view<functions::create_system>().each())
			create_system(destination_registry);
	}
}