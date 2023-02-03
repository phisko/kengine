#include "model_creator.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine data
#include "kengine/data/graphics.hpp"
#include "kengine/data/instance.hpp"
#include "kengine/data/model.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace kengine::systems {
	struct model_creator {
		entt::registry & r;
		entt::scoped_connection connection;

		model_creator(entt::handle e) noexcept
			: r(*e.registry()) {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "Init", "systems/model_creator");
			connection = r.on_construct<data::graphics>().connect<find_or_create_model>();
		}

		static void find_or_create_model(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & instance = r.get_or_emplace<data::instance>(e);
			if (instance.model != entt::null)
				return;

			const auto & graphics = r.get<data::graphics>(e);

			kengine_logf(r, log, "systems/model_creator", "Looking for model for %zu: %s", e, graphics.appearance.c_str());

			for (const auto & [model, comp] : r.view<data::model>().each())
				if (comp.file == graphics.appearance) {
					instance.model = model;
					kengine_logf(r, log, "systems/model_creator", "Found existing model: %zu", model);
					return;
				}

			const auto model = r.create();
			kengine_logf(r, log, "systems/model_creator", "Created new model: %zu", model);
			r.emplace<data::model>(model, graphics.appearance.c_str());
			instance.model = model;
		}
	};

	DEFINE_KENGINE_SYSTEM_CREATOR(model_creator)
}