#include "ModelCreatorSystem.hpp"
#include "kengine.hpp"

#include "data/GraphicsComponent.hpp"
#include "data/InstanceComponent.hpp"
#include "data/ModelComponent.hpp"

namespace kengine {
	EntityCreator * ModelCreatorSystem() noexcept {
		struct impl {
			static void init(Entity & e) noexcept {
				e += functions::OnEntityCreated{ onEntityCreated };
			}

			static void onEntityCreated(Entity & e) noexcept {
				const auto graphics = e.tryGet<GraphicsComponent>();
				if (!graphics)
					return;

				auto & instance = e.attach<InstanceComponent>();
				if (instance.model != INVALID_ID)
					return;

				for (const auto & [model, comp] : entities.with<ModelComponent>())
					if (comp.file == graphics->appearance) {
						instance.model = model.id;
						return;
					}

				entities += [&](Entity & model) noexcept {
					model += ModelComponent{ graphics->appearance.c_str() };
					instance.model = model.id;
				};
			}
		};

		return impl::init;
	}
}