#include "registerLoadFromJSON.hpp"

// putils
#include "reflection_helpers/json_helper.hpp"

// kengine meta
#include "meta/LoadFromJSON.hpp"

// kengine helpers
#include "helpers/registerMetaComponentImplementation.hpp"

namespace kengine {
	template<typename ... Comps>
	void registerLoadFromJSON() noexcept {
		registerMetaComponentImplementation<meta::LoadFromJSON, Comps...>(
			[](const auto t, const nlohmann::json & jsonEntity, Entity & e) noexcept {
				using Type = putils_wrapped_type(t);

				const auto it = jsonEntity.find(putils::reflection::get_class_name<Type>());
				if (it == jsonEntity.end())
					return;
				auto & comp = e.attach<Type>();
				putils::reflection::fromJSON(*it, comp);
			}
		);
	}
}

