#include "ModelCreatorSystem.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// kengine data
#include "data/GraphicsComponent.hpp"
#include "data/InstanceComponent.hpp"
#include "data/ModelComponent.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	struct ModelCreatorSystem {
		entt::registry & r;
		entt::scoped_connection connection;

		ModelCreatorSystem(entt::handle e) noexcept
			: r(*e.registry())
		{
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "ModelCreatorSystem");
			connection = r.on_construct<GraphicsComponent>().connect<findOrCreateModel>();
		}

		static void findOrCreateModel(entt::registry & r, entt::entity e) noexcept {
			KENGINE_PROFILING_SCOPE;

			auto & instance = r.get_or_emplace<InstanceComponent>(e);
			if (instance.model != entt::null)
				return;

			const auto & graphics = r.get<GraphicsComponent>(e);

			kengine_logf(r, Log, "ModelCreatorSystem", "Looking for model for %zu: %s", e, graphics.appearance.c_str());

			for (const auto & [model, comp] : r.view<ModelComponent>().each())
				if (comp.file == graphics.appearance) {
					instance.model = model;
					kengine_logf(r, Log, "ModelCreatorSystem", "Found existing model: %zu", model);
					return;
				}

			const auto model = r.create();
			kengine_logf(r, Log, "ModelCreatorSystem", "Created new model: %zu", model);
			r.emplace<ModelComponent>(model, graphics.appearance.c_str());
			instance.model = model;
		}
	};

	void addModelCreatorSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<ModelCreatorSystem>(e);
	}
}