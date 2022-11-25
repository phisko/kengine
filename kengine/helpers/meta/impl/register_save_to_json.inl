#include "register_save_to_json.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "putils/reflection_helpers/json_helper.hpp"

// kengine meta
#include "kengine/meta/save_to_json.hpp"

// kengine helpers
#include "kengine/helpers/register_meta_component_implementation.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace putils::reflection::detail::json {
	// Overload this for entt::entity as it's an enum and magic_enum doesn't like it
	template<>
	inline void from_to_json(nlohmann::json & json_object, const entt::entity & obj) noexcept {
		const auto non_enum_value = entt::id_type(obj);
		from_to_json(json_object, non_enum_value);
	}
}

namespace kengine {
	template<typename ... Comps>
	void register_save_to_json(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		register_meta_component_implementation<meta::save_to_json, Comps...>(
			r, [](const auto t, entt::const_handle e) noexcept {
				KENGINE_PROFILING_SCOPE;

				using type = putils_wrapped_type(t);

				if constexpr (std::is_empty<type>())
					return nlohmann::json{};
				else {
					const auto comp = e.try_get<type>();
					if (!comp)
						return nlohmann::json{};
					return putils::reflection::to_json(*comp);
				}
			}
		);
	}
}

