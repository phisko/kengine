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

		for (const auto e : r.view<T>())
			func({ r, e });
	}

	template<typename T>
	void meta_component_implementation<meta::for_each_entity_without, T>::function(entt::registry & r, const entity_iterator_func & func) noexcept {
		KENGINE_PROFILING_SCOPE;

		r.each([&](entt::entity e) {
			if (!r.all_of<T>(e))
				func({ r, e });
		});
	}
}