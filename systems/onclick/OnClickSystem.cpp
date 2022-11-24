#include "OnClickSystem.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

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
		OnClickSystem(entt::handle e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(*e.registry(), Log, "Init", "OnClickSystem");

			e.emplace<InputComponent>().onMouseButton = onMouseButton;
		}

		static void onMouseButton(entt::handle window, int button, const putils::Point2f & coords, bool pressed) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!pressed)
				return;

			const auto & r = *window.registry();
			kengine_logf(r, Log, "OnClickSystem", "Click in { %f, %f }", coords.x, coords.y);

			for (const auto & [_, getEntity] : r.view<functions::GetEntityInPixel>().each()) {
				const auto e = getEntity(window, coords);
				if (e == entt::null)
					continue;

				const auto onClick = r.try_get<functions::OnClick>(e);
				if (onClick) {
					kengine_logf(r, Log, "OnClickSystem", "Calling OnClick on %zu", e);
					onClick->call(button);
				}
				else
					kengine_logf(r, Log, "OnClickSystem", "Clicked %zu, did not have OnClick", e);
			}
		}
	};

	void addOnClickSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<OnClickSystem>(e);
	}
}
