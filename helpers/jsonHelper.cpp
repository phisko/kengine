#include "jsonHelper.hpp"

#include "data/NameComponent.hpp"
#include "meta/Has.hpp"
#include "meta/LoadFromJSON.hpp"
#include "meta/SaveToJSON.hpp"

#include "helpers/sortHelper.hpp"

namespace kengine::jsonHelper {
	Entity createEntity(const putils::json & entityJSON) noexcept {
		return entities.create([&](Entity & e) {
			loadEntity(entityJSON, e);
		});
	}

	void loadEntity(const putils::json & entityJSON, Entity & e) noexcept {
		for (const auto & [_, loader] : entities.with<meta::LoadFromJSON>())
			loader(entityJSON, e);
	}

	putils::json saveEntity(const Entity & e) noexcept {
		putils::json ret;

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