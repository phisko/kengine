#include "json_helper.hpp"

// stl
#include <algorithm>
#include <execution>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/range.hpp"

// kengine core
#include "kengine/core/data/name.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/core/sort/helpers/get_name_sorted_entities.hpp"

// kengine meta
#include "kengine/meta/functions/has.hpp"

// kengine meta/json
#include "kengine/meta/json/functions/load_from_json.hpp"
#include "kengine/meta/json/functions/save_to_json.hpp"

namespace kengine::json_helper {
	void load_entity(const nlohmann::json & entity_json, entt::handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), verbose, "json_helper", "Loading [%u] from JSON", e.entity());
		kengine_logf(*e.registry(), very_verbose, "json_helper", "Input: %s", entity_json.dump(4).c_str());

		const auto view = e.registry()->view<const meta::load_from_json>();
		std::for_each(std::execution::par_unseq, putils_range(view), [&](entt::entity loader_entity) {
			const auto & [loader] = view.get(loader_entity);
			loader(entity_json, e);
		});
	}

	nlohmann::json save_entity(entt::const_handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), verbose, "json_helper", "Saving [%u] to JSON", e.entity());

		nlohmann::json ret;

		const auto types = core::sort::get_name_sorted_entities<const meta::has, const meta::save_to_json>(*e.registry());

		for (const auto & [_, name, has, save] : types) {
			if (!has->call(e))
				continue;

			ret[name->name.c_str()] = save->call(e);
		}

		kengine_logf(*e.registry(), very_verbose, "json_helper", "Output: %s", ret.dump(4).c_str());
		return ret;
	}
}