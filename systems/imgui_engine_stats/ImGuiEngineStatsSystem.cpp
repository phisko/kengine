#include "ImGuiEngineStatsSystem.hpp"

#include "kengine.hpp"
#include "impl/GlobalState.hpp"

#include "data/NameComponent.hpp"
#include "data/ImGuiToolComponent.hpp"
#include "functions/Execute.hpp"

#include "imgui.h"

namespace kengine {
	EntityCreator * ImGuiEngineStatsSystem() noexcept {
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
					const auto entityCount = std::count_if(
						kengine::impl::state->_entities.begin(), kengine::impl::state->_entities.end(),
						[](const kengine::impl::GlobalState::EntityMetadata & e) {
							return e.active && e.mask != 0;
						}
					);

					ImGui::Text("Entities: %zu", entityCount);
					ImGui::Text("\tEntity pool size: %zu", kengine::impl::state->_entities.size());
					ImGui::Text("Archetypes: %zu", kengine::impl::state->_archetypes.size());
					ImGui::Text("Component types: %zu", kengine::impl::state->_components.size());
				}
				ImGui::End();
			}
		};

		return impl::init;
	}
}
