#include "OnClickSystem.hpp"
#include "kengine.hpp"

#include "data/OnClickComponent.hpp"
#include "data/InputComponent.hpp"
#include "functions/GetEntityInPixel.hpp"

namespace kengine::onclick {
	struct impl {
		static void init(Entity & e) noexcept {
			InputComponent input;
			input.onMouseButton = onMouseButton;
			e += input;
		}

		static void onMouseButton(EntityID window, int button, const putils::Point2f & coords, bool pressed) noexcept {
			if (!pressed)
				return;

			for (const auto & [e, getEntity] : entities.with<functions::GetEntityInPixel>()) {
				const auto id = getEntity(window, coords);
				if (id == INVALID_ID)
					continue;

				const auto e = entities.get(id);

				const auto onClick = e.tryGet<OnClickComponent>();
				if (onClick)
					onClick->onClick();
			}
		}
	};
}

namespace kengine {
	EntityCreator * OnClickSystem() noexcept {
		return [](Entity & e) noexcept {
			onclick::impl::init(e);
		};
	}
}
