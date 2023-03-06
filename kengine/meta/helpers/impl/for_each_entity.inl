#include "for_each_entity.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace kengine::meta {
	template<typename T>
	void meta_component_implementation<for_each_entity, T>::function(entt::registry & r, const entity_iterator_func & func) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, very_verbose, "meta::for_each_entity", "Iterating over all entities with {}", putils::reflection::get_class_name<T>());

		for (const auto e : r.view<T>()) {
			kengine_logf(r, very_verbose, "meta::for_each_entity", "Found {}", e);
			func({ r, e });
		}
	}

	template<typename T>
	void meta_component_implementation<for_each_entity_without, T>::function(entt::registry & r, const entity_iterator_func & func) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(r, very_verbose, "meta::for_each_entity_without", "Iterating over all entities without {}", putils::reflection::get_class_name<T>());

		r.each([&](entt::entity e) {
			if (!r.all_of<T>(e)) {
				kengine_logf(r, very_verbose, "meta::for_each_entity_without", "Found {}", e);
				func({ r, e });
			}
		});
	}
}