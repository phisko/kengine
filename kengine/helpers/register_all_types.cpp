#include "register_all_types.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine functions
#include "kengine/functions/register_types.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	struct pre_registered {};

	void pre_register_all_types(entt::registry & destination_registry, const entt::registry * main_registry) noexcept {
		KENGINE_PROFILING_SCOPE;

		kengine_log(destination_registry, log, "register_all_types", "Pre-registering all known types");

		if (!main_registry)
			main_registry = &destination_registry;

		destination_registry.emplace<pre_registered>(destination_registry.create());

		for (const auto & [e, pre_register_types] : main_registry->view<functions::pre_register_types>().each())
			pre_register_types(destination_registry);
	}

	void register_all_types(entt::registry & destination_registry, const entt::registry * main_registry) noexcept {
		KENGINE_PROFILING_SCOPE;

		kengine_log(destination_registry, log, "register_all_types", "Registering all known types");

		if (destination_registry.view<pre_registered>().empty())
			pre_register_all_types(destination_registry, main_registry);

		if (!main_registry)
			main_registry = &destination_registry;

		for (const auto & [e, register_types] : main_registry->view<functions::register_types>().each())
			register_types(destination_registry);
	}
}