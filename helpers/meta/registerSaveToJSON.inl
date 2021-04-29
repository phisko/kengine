#include "registerSaveToJSON.hpp"

#include "meta/SaveToJSON.hpp"
#include "helpers/registerMetaComponentImplementation.hpp"
#include "reflection_helpers/json_helper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerSaveToJSON() noexcept {
		registerMetaComponentImplementation<meta::SaveToJSON, Comps...>(
			[](const auto t, const Entity & e) noexcept {
				using Type = putils_wrapped_type(t);
				const auto comp = e.tryGet<Type>();
				if (!comp)
					return putils::json{};
				return putils::reflection::toJSON(*comp);
			}
		);
	}
}

