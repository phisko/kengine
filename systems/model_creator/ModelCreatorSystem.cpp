#include "ModelCreatorSystem.hpp"
#include "kengine.hpp"

#include "data/GraphicsComponent.hpp"
#include "data/InstanceComponent.hpp"
#include "data/ModelComponent.hpp"

#include "helpers/logHelper.hpp"

namespace kengine {
	EntityCreator * ModelCreatorSystem() noexcept {
		struct impl {
			static void init(Entity & e) noexcept {
				kengine_log(Log, "Init", "ModelCreatorSystem");
				e += functions::OnEntityCreated{ onEntityCreated };
			}

			static void onEntityCreated(Entity & e) noexcept {
				const auto graphics = e.tryGet<GraphicsComponent>();
				if (!graphics)
					return;

				auto & instance = e.attach<InstanceComponent>();
				if (instance.model != INVALID_ID)
					return;

				kengine_logf(Log, "ModelCreatorSystem", "Looking for model for %zu: %s", e.id, graphics->appearance.c_str());

				for (const auto & [model, comp] : entities.with<ModelComponent>())
					if (comp.file == graphics->appearance) {
						instance.model = model.id;
						kengine_logf(Log, "ModelCreatorSystem", "Found existing model: %zu", model.id);
						return;
					}

				entities += [&](Entity & model) noexcept {
					kengine_logf(Log, "ModelCreatorSystem", "Created new model: %zu", model.id);
					model += ModelComponent{ graphics->appearance.c_str() };
					instance.model = model.id;
				};
			}
		};

		return impl::init;
	}
}