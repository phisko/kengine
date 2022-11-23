#include "registerLoadFromJSON.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "reflection_helpers/json_helper.hpp"

// kengine meta
#include "meta/LoadFromJSON.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace putils::reflection::detail::json {
	// Overload this for entt::entity as it's an enum and magic_enum doesn't like it
	template<>
	inline void fromToJSON(const nlohmann::json & jsonObject, entt::entity & obj) noexcept {
		auto nonEnumValue = entt::id_type(obj);
		fromToJSON(jsonObject, nonEnumValue);
		obj = entt::entity(nonEnumValue);
	}
}

namespace kengine {
	template<typename ... Comps>
	void registerLoadFromJSON(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		registerMetaComponentImplementation<meta::LoadFromJSON, Comps...>(
			r, [](const auto t, const nlohmann::json & jsonEntity, entt::handle e) noexcept {
				KENGINE_PROFILING_SCOPE;

				using Type = putils_wrapped_type(t);

				const auto it = jsonEntity.find(putils::reflection::get_class_name<Type>());
				if (it == jsonEntity.end())
					return;

				if constexpr (!std::is_empty<Type>()) {
					Type comp;
					putils::reflection::fromJSON(*it, comp);
					e.emplace_or_replace<Type>(std::move(comp));
				}
				else
					e.emplace<Type>();
			}
		);
	}
}

