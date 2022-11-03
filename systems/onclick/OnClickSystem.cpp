#include "OnClickSystem.hpp"
#include "kengine.hpp"

// kengine data
#include "data/InputComponent.hpp"

// kengine functions
#include "functions/GetEntityInPixel.hpp"
#include "functions/OnClick.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"

namespace kengine::onclick {
	struct impl {
		static void init(Entity & e) noexcept {
			kengine_log(Log, "Init", "OnClickSystem");
			InputComponent input;
			input.onMouseButton = onMouseButton;
			e += input;
		}

		static void onMouseButton(EntityID window, int button, const putils::Point2f & coords, bool pressed) noexcept {
			if (!pressed)
				return;

			kengine_logf(Log, "OnClickSystem", "Click in { %f, %f }", coords.x, coords.y);

			for (const auto & [_, getEntity] : entities.with<functions::GetEntityInPixel>()) {
				const auto id = getEntity(window, coords);
				if (id == INVALID_ID)
					continue;

				const auto e = entities[id];

				const auto onClick = e.tryGet<functions::OnClick>();
				if (onClick) {
					kengine_logf(Log, "OnClickSystem", "Calling OnClick on %zu", id);
					onClick->call(button);
				}
				else
					kengine_logf(Log, "OnClickSystem", "Clicked %zu, did not have OnClick", id);
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
