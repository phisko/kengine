#include "has_metadata.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/core/log/helpers/kengine_log.hpp"

namespace kengine::meta {
	template<typename T>
	bool meta_component_implementation<has_metadata, T>::function(std::string_view key) noexcept {
		return putils::reflection::has_metadata<T>(key);
	}
}