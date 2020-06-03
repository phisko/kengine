#include "OnClickSystem.hpp"
#include "EntityManager.hpp"

#include "data/OnClickComponent.hpp"
#include "data/InputComponent.hpp"
#include "functions/GetEntityInPixel.hpp"

namespace kengine {
	EntityCreatorFunctor<64> OnClickSystem(EntityManager & em) {
		return [&](Entity & e) {
			InputComponent input;
			input.onMouseButton = [&](Entity::ID window, int button, const putils::Point2f & coords, bool pressed) {
				if (!pressed)
					return;

				for (const auto & [e, getEntity] : em.getEntities<functions::GetEntityInPixel>()) {
					const auto id = getEntity(window, coords);
					if (id == Entity::INVALID_ID)
						continue;
					auto & e = em.getEntity(id);
					if (e.has<OnClickComponent>())
						e.get<OnClickComponent>().onClick();
				}
			};
			e += input;
		};
	}
}
