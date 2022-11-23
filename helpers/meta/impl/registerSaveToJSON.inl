#include "registerSaveToJSON.hpp"

// entt
#include <entt/entity/handle.hpp>

// putils
#include "reflection_helpers/json_helper.hpp"

// kengine meta
#include "meta/SaveToJSON.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace putils::reflection::detail::json {
	// Overload this for entt::entity as it's an enum and magic_enum doesn't like it
	template<>
	inline void fromToJSON(nlohmann::json & jsonObject, const entt::entity & obj) noexcept {
		const auto nonEnumValue = entt::id_type(obj);
		fromToJSON(jsonObject, nonEnumValue);
	}
}

namespace kengine {
	template<typename ... Comps>
	void registerSaveToJSON(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		registerMetaComponentImplementation<meta::SaveToJSON, Comps...>(
			r, [](const auto t, entt::const_handle e) noexcept {
				KENGINE_PROFILING_SCOPE;

				using Type = putils_wrapped_type(t);

				if constexpr (std::is_empty<Type>())
					return nlohmann::json{};
				else {
					const auto comp = e.try_get<Type>();
					if (!comp)
						return nlohmann::json{};
					return putils::reflection::toJSON(*comp);
				}
			}
		);
	}
}

