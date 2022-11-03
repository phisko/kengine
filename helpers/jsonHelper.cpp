#include "jsonHelper.hpp"

// kengine data
#include "data/NameComponent.hpp"

// kengine meta
#include "meta/Has.hpp"
#include "meta/LoadFromJSON.hpp"
#include "meta/SaveToJSON.hpp"

// kengine helpers
#include "helpers/sortHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine::jsonHelper {
	Entity createEntity(const nlohmann::json & entityJSON) noexcept {
		KENGINE_PROFILING_SCOPE;
		return entities.create([&](Entity & e) {
			loadEntity(entityJSON, e);
		});
	}

	void loadEntity(const nlohmann::json & entityJSON, Entity & e) noexcept {
		KENGINE_PROFILING_SCOPE;
		for (const auto & [_, loader] : entities.with<meta::LoadFromJSON>())
			loader(entityJSON, e);
	}

	nlohmann::json saveEntity(const Entity & e) noexcept {
		KENGINE_PROFILING_SCOPE;

		nlohmann::json ret;

		const auto types = sortHelper::getNameSortedEntities<KENGINE_COMPONENT_COUNT,
			meta::Has, meta::SaveToJSON
		>();

		for (const auto & [_, name, has, save] : types) {
			if (!has->call(e))
				continue;

			ret[name->name.c_str()] = save->call(e);
		}

		return ret;
	}
}