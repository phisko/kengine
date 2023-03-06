#include "new_entity_processor.hpp"

// entt
#include <entt/entity/registry.hpp>

// meta
#include "putils/meta/fwd.hpp"

// kengine
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"
#include "kengine/meta/helpers/register_storage.hpp"

namespace kengine {
	template<typename ProcessedTag, typename... Comps>
	new_entity_processor<ProcessedTag, Comps...>::new_entity_processor(entt::registry & r, const callback_type & callback) noexcept
		: r(r),
		  callback(callback) {
		if (!meta::is_storage_registered<ProcessedTag>(r)) {
			kengine_log(r, verbose, "new_entity_processor", "Processed tag wasn't pre-registered. Consider pre-registering it to avoid potential race conditions.");
			meta::register_storage<ProcessedTag>(r);
		}
	}

	template<typename ProcessedTag, typename... Comps>
	void new_entity_processor<ProcessedTag, Comps...>::process() noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, very_verbose, "new_entity_processor", "Processing new entities");

		const auto view = r.view<Comps...>(entt::exclude<ProcessedTag>);
		for (const auto e : view) {
			kengine_logf(r, very_verbose, "new_entity_processor", "Processing {}", e);

			r.emplace<ProcessedTag>(e);
			std::apply(
				callback,
				std::tuple_cat(std::make_tuple(e), view.get(e))
			);
		}
	}
}