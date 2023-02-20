#include "model_creator.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/forward_to.hpp"

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/helpers/new_entity_processor.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine model_instance
#include "kengine/model_instance/data/graphics.hpp"
#include "kengine/model_instance/data/instance.hpp"
#include "kengine/model_instance/data/model.hpp"

// kengine main_loop
#include "kengine/main_loop/functions/execute.hpp"

namespace kengine::systems {
	struct model_creator {
		entt::registry & r;

		struct processed {};
		kengine::new_entity_processor<processed, data::graphics> processor{ r, putils_forward_to_this(find_or_create_model) };

		model_creator(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "model_creator", "Initializing");
			e.emplace<functions::execute>(putils_forward_to_this(execute));

			processor.process();
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "model_creator", "Executing");
			processor.process();
		}

		void find_or_create_model(entt::entity e, const data::graphics & graphics) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & instance = r.get_or_emplace<data::instance>(e);
			if (instance.model != entt::null) {
				kengine_logf(r, verbose, "model_creator", "[%u] already has a model ([%u])", instance.model);
				return;
			}

			kengine_logf(r, verbose, "model_creator", "Looking for model for [%u] (%s)", e, graphics.appearance.c_str());

			for (const auto & [model, comp] : r.view<data::model>().each())
				if (comp.file == graphics.appearance) {
					instance.model = model;
					kengine_logf(r, verbose, "model_creator", "Found existing model ([%u])", model);
					return;
				}

			const auto model = r.create();
			kengine_logf(r, log, "model_creator", "Created new model [%u] for %s", model, graphics.appearance.c_str());
			r.emplace<data::model>(model, graphics.appearance.c_str());
			instance.model = model;
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(
		model_creator,
		model_creator::processed
	)
}