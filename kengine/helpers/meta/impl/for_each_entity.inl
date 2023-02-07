#include "for_each_entity.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine helpers
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine {
	template<typename T>
	void meta_component_implementation<meta::for_each_entity, T>::function(entt::registry & r, const entity_iterator_func & func) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, very_verbose, "meta::for_each_entity", "Iterating over all entities with %s", putils::reflection::get_class_name<T>());

		for (const auto e : r.view<T>()) {
			kengine_logf(r, very_verbose, "meta::for_each_entity", "Found [%zu]", e);
			func({ r, e });
		}
	}

	template<typename T>
	void meta_component_implementation<meta::for_each_entity_without, T>::function(entt::registry & r, const entity_iterator_func & func) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, very_verbose, "meta::for_each_entity_without", "Iterating over all entities without %s", putils::reflection::get_class_name<T>());

		r.each([&](entt::entity e) {
			if (!r.all_of<T>(e)) {
				kengine_logf(r, very_verbose, "meta::for_each_entity_without", "Found [%zu]", e);
				func({ r, e });
			}
		});
	}
}