#include "OnClickSystem.hpp"
#include "kengine.hpp"

// kengine data
#include "data/InputComponent.hpp"

// kengine functions
#include "functions/GetEntityInPixel.hpp"
#include "functions/OnClick.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	struct OnClickSystem {
		static void init(Entity & e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Log, "Init", "OnClickSystem");

			InputComponent input;
			input.onMouseButton = onMouseButton;
			e += input;
		}

		static void onMouseButton(EntityID window, int button, const putils::Point2f & coords, bool pressed) noexcept {
			KENGINE_PROFILING_SCOPE;

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

	EntityCreator * OnClickSystem() noexcept {
		return OnClickSystem::init;
	}
}
