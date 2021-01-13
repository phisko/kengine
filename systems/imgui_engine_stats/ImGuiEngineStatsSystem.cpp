#include "ImGuiEngineStats.hpp"

#include "kengine.hpp"
#include "impl/GlobalState.hpp"

#include "data/NameComponent.hpp"
#include "data/ImGuiToolComponent.hpp"
#include "functions/Execute.hpp"

#include "imgui.h"

namespace kengine {
	EntityCreator * ImGuiEngineStats() noexcept {
		static bool * enabled;

		struct impl {
			static void init(Entity & e) noexcept {
				e += NameComponent{ "Engine stats" };
				auto & tool = e.attach<ImGuiToolComponent>();
				tool.enabled = true;
				enabled = &tool.enabled;

				e += functions::Execute{ execute };
			}

			static void execute(float deltaTime) noexcept {
				if (!*enabled)
					return;

				if (ImGui::Begin("Engine stats")) {
					ImGui::Text("Entities: %zu", kengine::impl::state->_entities.size());
					ImGui::Text("Archetypes: %zu", kengine::impl::state->_archetypes.size());
				}
				ImGui::End();
			}
		};

		return impl::init;
	}
}
