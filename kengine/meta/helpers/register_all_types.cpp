#include "register_all_types.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine meta
#include "kengine/meta/functions/register_types.hpp"

namespace kengine::meta {
	static constexpr auto log_category = "meta";

	struct pre_registered {};

	void pre_register_all_types(entt::registry & destination_registry, const entt::registry * main_registry) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(destination_registry, log, log_category, "Pre-registering all known types");

		if (!main_registry)
			main_registry = &destination_registry;

		destination_registry.emplace<pre_registered>(destination_registry.create());

		for (const auto & [e, pre_register_types] : main_registry->view<functions::pre_register_types>().each()) {
			kengine_logf(destination_registry, very_verbose, log_category, "Found registration in [%u]", e);
			pre_register_types(destination_registry);
		}
	}

	void register_all_types(entt::registry & destination_registry, const entt::registry * main_registry) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(destination_registry, log, log_category, "Registering all known types");

		if (destination_registry.view<pre_registered>().empty())
			pre_register_all_types(destination_registry, main_registry);

		if (!main_registry)
			main_registry = &destination_registry;

		for (const auto & [e, register_types] : main_registry->view<functions::register_types>().each()) {
			kengine_logf(destination_registry, very_verbose, log_category, "Found registration in [%u]", e);
			register_types(destination_registry);
		}
	}
}