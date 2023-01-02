#include "register_load_from_json.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "putils/reflection_helpers/json_helper.hpp"

// kengine meta
#include "kengine/meta/load_from_json.hpp"

// kengine helpers
#include "kengine/helpers/register_meta_component_implementation.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace putils::reflection::detail::json {
	// Overload this for entt::entity as it's an enum and magic_enum doesn't like it
	template<>
	inline void from_to_json(const nlohmann::json & json_object, entt::entity & obj) noexcept {
		auto non_enum_value = entt::id_type(obj);
		from_to_json(json_object, non_enum_value);
		obj = entt::entity(non_enum_value);
	}
}

namespace kengine {
	template<typename... Comps>
	void register_load_from_json(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		register_meta_component_implementation<meta::load_from_json, Comps...>(
			r, [](const auto t, const nlohmann::json & jsonEntity, entt::handle e) noexcept {
				KENGINE_PROFILING_SCOPE;

				using type = putils_wrapped_type(t);

				const auto it = jsonEntity.find(putils::reflection::get_class_name<type>());
				if (it == jsonEntity.end())
					return;

				if constexpr (!std::is_empty<type>()) {
					type comp;
					putils::reflection::from_json(*it, comp);
					e.emplace_or_replace<type>(std::move(comp));
				}
				else
					e.emplace<type>();
			}
		);
	}
}
