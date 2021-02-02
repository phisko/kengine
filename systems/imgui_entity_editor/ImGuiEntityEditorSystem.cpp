#include "ImGuiEntityEditorSystem.hpp"
#include "kengine.hpp"

#include "data/ImGuiToolComponent.hpp"
#include "data/InstanceComponent.hpp"
#include "data/SelectedComponent.hpp"
#include "data/NameComponent.hpp"

#include "functions/Execute.hpp"

#include "helpers/typeHelper.hpp"
#include "helpers/imGuiHelper.hpp"
#include "imgui.h"

namespace kengine {
	EntityCreator * ImGuiEntityEditorSystem() noexcept {
		static bool * enabled;

		struct impl {
			static void init(Entity & e) noexcept {
				e += NameComponent{ "Entity editor" };
				auto & tool = e.attach<ImGuiToolComponent>();
				tool.enabled = true;
				enabled = &tool.enabled;

				e += functions::Execute{ execute };
			}

			static void execute(float deltaTime) noexcept {
				if (!*enabled)
					return;

				const auto scale = imguiHelper::getScale();
				for (auto [selected, _] : entities.with<SelectedComponent>()) {
					bool open = true;

					ImGui::SetNextWindowSize({ 200.f * scale, 200.f * scale }, ImGuiCond_FirstUseEver);

					const auto name = selected.tryGet<NameComponent>();
					const auto windowTitle =
						name ?
						putils::string<64>("%s##[%d]", name->name.c_str(), selected.id) :
						putils::string<64>("[%d] Entity editor", selected.id);

					if (ImGui::Begin(windowTitle, &open, ImGuiWindowFlags_NoSavedSettings))
						imguiHelper::editEntityAndModel(selected);
					ImGui::End();

					if (!open)
						selected.detach<SelectedComponent>();
				}
			}
		};

		return impl::init;
	}
}
