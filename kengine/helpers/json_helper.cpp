#include "json_helper.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine data
#include "kengine/data/name.hpp"

// kengine meta
#include "kengine/meta/has.hpp"
#include "kengine/meta/load_from_json.hpp"
#include "kengine/meta/save_to_json.hpp"

// kengine helpers
#include "kengine/helpers/sort_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::json_helper {
	void load_entity(const nlohmann::json & entity_json, entt::handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		for (const auto & [_, loader] : e.registry()->view<const meta::load_from_json>().each())
			loader(entity_json, e);
	}

	nlohmann::json save_entity(entt::const_handle e) noexcept {
		KENGINE_PROFILING_SCOPE;

		nlohmann::json ret;

		const auto types = sort_helper::get_name_sorted_entities<
			const meta::has, const meta::save_to_json
		>(*e.registry());

		for (const auto & [_, name, has, save] : types) {
			if (!has->call(e))
				continue;

			ret[name->name.c_str()] = save->call(e);
		}

		return ret;
	}
}