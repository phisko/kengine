#pragma

// entt
#include <entt/entity/fwd.hpp>

// nlohmann
#include <nlohmann/json.hpp>

// putils
#include "putils/reflection_helpers/json_helper.hpp"

namespace kengine::json_helper {
	KENGINE_CORE_EXPORT void load_entity(const nlohmann::json & entity_json, entt::handle e) noexcept;
	KENGINE_CORE_EXPORT nlohmann::json save_entity(entt::const_handle e) noexcept;
}

namespace putils::reflection::detail::json {
	// Overload these for entt::entity as it's an enum and magic_enum doesn't like it
	template<>
	KENGINE_CORE_EXPORT void from_to_json(const nlohmann::json & json_object, entt::entity & obj) noexcept;

	template<>
	KENGINE_CORE_EXPORT void from_to_json(nlohmann::json & json_object, const entt::entity & obj) noexcept;
}