#include "registerSaveToJSON.hpp"

// putils
#include "reflection_helpers/json_helper.hpp"

// kengine meta
#include "meta/SaveToJSON.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerSaveToJSON() noexcept {
		registerMetaComponentImplementation<meta::SaveToJSON, Comps...>(
			[](const auto t, const Entity & e) noexcept {
				using Type = putils_wrapped_type(t);
				const auto comp = e.tryGet<Type>();
				if (!comp)
					return nlohmann::json{};
				return putils::reflection::toJSON(*comp);
			}
		);
	}
}

