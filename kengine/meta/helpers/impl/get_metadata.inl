#include "get_metadata.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/core/log/helpers/kengine_log.hpp"

namespace kengine::meta {
	template<typename T>
	const void * meta_component_implementation<get_metadata, T>::function(std::string_view key) noexcept {
		return putils::reflection::get_metadata<void, T>(key);
	}
}
