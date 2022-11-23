#include "jsonHelper.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

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
	void loadEntity(const nlohmann::json & entityJSON, entt::handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		for (const auto & [_, loader] : e.registry()->view<const meta::LoadFromJSON>().each())
			loader(entityJSON, e);
	}

	nlohmann::json saveEntity(entt::const_handle e) noexcept {
		KENGINE_PROFILING_SCOPE;

		nlohmann::json ret;

		const auto types = sortHelper::getNameSortedEntities<
			const meta::Has, const meta::SaveToJSON
		>(*e.registry());

		for (const auto & [_, name, has, save] : types) {
			if (!has->call(e))
				continue;

			ret[name->name.c_str()] = save->call(e);
		}

		return ret;
	}
}