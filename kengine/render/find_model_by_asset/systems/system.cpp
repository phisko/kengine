#include "system.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/forward_to.hpp"

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/helpers/new_entity_processor.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine instance
#include "kengine/instance/data/instance.hpp"

// kengine main_loop
#include "kengine/main_loop/functions/execute.hpp"

// kengine render/find_model_by_asset
#include "kengine/render/find_model_by_asset/data/instance_of_asset.hpp"

namespace kengine::render::find_model_by_asset {
	static constexpr auto log_category = "render_find_model_by_asset";

	struct system {
		entt::registry & r;

		struct processed {};
		kengine::new_entity_processor<processed, instance_of_asset> processor{ r, putils_forward_to_this(find_or_create_model) };

		system(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, log_category, "Initializing");
			e.emplace<main_loop::execute>(putils_forward_to_this(execute));

			processor.process();
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, log_category, "Executing");
			processor.process();
		}

		void find_or_create_model(entt::entity e, const instance_of_asset & asset_name) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & comp = r.get_or_emplace<instance::instance>(e);
			if (comp.model != entt::null) {
				kengine_logf(r, verbose, log_category, "[%u] already has a model ([%u])", comp.model);
				return;
			}

			kengine_logf(r, verbose, log_category, "Looking for model for [%u] (asset: %s)", e, asset_name.asset.c_str());

			for (const auto & [model, asset] : r.view<render::asset>().each())
				if (asset.file == asset_name.asset) {
					comp.model = model;
					kengine_logf(r, verbose, log_category, "Found existing model ([%u])", model);
					return;
				}

			const auto model = r.create();
			kengine_logf(r, log, log_category, "Created new model [%u] for name %s", model, asset_name.asset.c_str());
			r.emplace<render::asset>(model, asset_name.asset.c_str());
			comp.model = model;
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		system,
		system::processed
	)
}