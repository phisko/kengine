#include "ImGuiEntityEditorSystem.hpp"
#include "kengine.hpp"

// imgui
#include <imgui.h>

// kengine data
#include "data/ImGuiToolComponent.hpp"
#include "data/InstanceComponent.hpp"
#include "data/SelectedComponent.hpp"
#include "data/NameComponent.hpp"

// kengine functions
#include "functions/Execute.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/imguiHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace kengine {
	struct ImGuiEntityEditorSystem {
		static void init(Entity & e) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(Log, "Init", "ImGuiEntityEditorSystem");

			e += NameComponent{ "Entity editor" };
			auto & tool = e.attach<ImGuiToolComponent>();
			tool.enabled = true;
			_enabled = &tool.enabled;

			e += functions::Execute{ execute };
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!*_enabled)
				return;

			kengine_log(Verbose, "Execute", "ImGuiEntityEditorSystem");
			const auto scale = imguiHelper::getScale();
			for (auto [selected, _] : entities.with<SelectedComponent>()) {
				bool open = true;

				ImGui::SetNextWindowSize({ 200.f * scale, 200.f * scale }, ImGuiCond_FirstUseEver);

				const auto name = selected.tryGet<NameComponent>();
				const auto windowTitle =
					name ?
					putils::string<64>("%s##[%d]", name->name.c_str(), selected.id) :
					putils::string<64>("[%d] Entity editor", selected.id);

				if (ImGui::Begin(windowTitle.c_str(), &open, ImGuiWindowFlags_NoSavedSettings))
					imguiHelper::editEntityAndModel(selected);
				ImGui::End();

				if (!open)
					selected.detach<SelectedComponent>();
			}
		}

		static inline bool * _enabled;
	};

	EntityCreator * ImGuiEntityEditorSystem() noexcept {
		return ImGuiEntityEditorSystem::init;
	}
}
