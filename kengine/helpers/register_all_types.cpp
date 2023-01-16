#include "register_all_types.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine functions
#include "kengine/functions/register_types.hpp"

// kengine helpers
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	void register_all_types(entt::registry & destination_registry, const entt::registry * main_registry) noexcept {
		KENGINE_PROFILING_SCOPE;

		if (!main_registry)
			main_registry = &destination_registry;

		for (const auto & [e, register_types] : main_registry->view<functions::register_types>().each())
			register_types(destination_registry);
	}
}