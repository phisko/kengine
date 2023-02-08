#include "model_creator.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// putils
#include "putils/forward_to.hpp"

// kengine data
#include "kengine/data/graphics.hpp"
#include "kengine/data/instance.hpp"
#include "kengine/data/model.hpp"

// kengine functions
#include "kengine/functions/execute.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::systems {
	struct model_creator {
		entt::registry & r;

		model_creator(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "model_creator", "Initializing");
			e.emplace<functions::execute>(putils_forward_to_this(execute));

			process_new_entities();
		}

		void execute(float delta_time) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "model_creator", "Executing");
			process_new_entities();
		}

		void process_new_entities() noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, very_verbose, "model_creator", "Processing new entities");

			for (const auto & [e, graphics] : r.view<data::graphics>(entt::exclude<data::instance>).each())
				find_or_create_model(e, graphics);
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

	DEFINE_KENGINE_SYSTEM_CREATOR(model_creator)
}