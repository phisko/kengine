#include "save_entity.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine
#include "kengine/core/data/name.hpp"
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/meta/functions/has.hpp"
#include "kengine/meta/json/functions/save.hpp"

namespace kengine::meta::json {
	static constexpr auto log_category = "meta_json";

	nlohmann::json save_entity(entt::const_handle e) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_logf(*e.registry(), verbose, log_category, "Saving {} to JSON", e);

		nlohmann::json ret;

		for (const auto & [type_entity, name, has, save_to_json] : e.registry()->view<core::name, meta::has, save>().each()) {
			if (!has(e))
				continue;
			ret[name.name.c_str()] = save_to_json(e);
		}

		kengine_logf(*e.registry(), very_verbose, log_category, "Output: {}", ret.dump(4));
		return ret;
	}
}