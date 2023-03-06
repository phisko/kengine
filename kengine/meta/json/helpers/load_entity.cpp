#include "load_entity.hpp"

// stl
#include <algorithm>
#include <execution>

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/range.hpp"

// kengine
#include "kengine/core/data/name.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/meta/json/functions/load.hpp"

namespace kengine::meta::json {
	static constexpr auto log_category = "meta_json";

	void load_entity(const nlohmann::json & entity_json, entt::handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), verbose, log_category, "Loading {} from JSON", e);
		kengine_logf(*e.registry(), very_verbose, log_category, "Input: {}", entity_json.dump(4));

		const auto view = e.registry()->view<const load>();
		std::for_each(std::execution::par_unseq, putils_range(view), [&](entt::entity loader_entity) {
			const auto & [loader] = view.get(loader_entity);
			loader(entity_json, e);
		});
	}
}