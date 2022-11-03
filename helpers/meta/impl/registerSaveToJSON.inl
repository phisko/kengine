#include "registerSaveToJSON.hpp"

// putils
#include "reflection_helpers/json_helper.hpp"

// kengine meta
#include "meta/SaveToJSON.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerSaveToJSON() noexcept {
		KENGINE_PROFILING_SCOPE;

		registerMetaComponentImplementation<meta::SaveToJSON, Comps...>(
			[](const auto t, const Entity & e) noexcept {
				KENGINE_PROFILING_SCOPE;

				using Type = putils_wrapped_type(t);
				const auto comp = e.tryGet<Type>();
				if (!comp)
					return nlohmann::json{};
				return putils::reflection::toJSON(*comp);
			}
		);
	}
}

