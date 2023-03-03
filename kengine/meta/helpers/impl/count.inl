#include "count.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"

namespace kengine::meta {
	template<typename T>
	size_t meta_component_implementation<count, T>::function(entt::registry & r) noexcept {
		kengine_logf(r, very_verbose, "meta::count", "Counting number of {}", putils::reflection::get_class_name<T>());

		const auto view = r.view<T>();
		if constexpr (requires { view.size(); }) { // in-place-delete views don't have size()
			kengine_log(r, very_verbose, "meta::count", "Using view.size()");
			return view.size();
		}
		else {
			kengine_log(r, very_verbose, "meta::count", "Using view.size_hint()");
			return view.size_hint();
		}
	}
}
