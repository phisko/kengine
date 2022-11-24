#include "ImGuiEntityEditorSystem.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

// imgui
#include <imgui.h>

// putils
#include "forward_to.hpp"

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
		entt::registry & r;
		bool * enabled;

		ImGuiEntityEditorSystem(entt::handle e) noexcept
			: r(*e.registry())
		{
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "ImGuiEntityEditorSystem");

			e.emplace<functions::Execute>(putils_forward_to_this(execute));

			e.emplace<NameComponent>("Entity editor");
			auto & tool = e.emplace<ImGuiToolComponent>(true);
			enabled = &tool.enabled;
		}

		void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!*enabled)
				return;

			kengine_log(r, Verbose, "Execute", "ImGuiEntityEditorSystem");
			const auto scale = imguiHelper::getScale(r);
			for (auto [selected] : r.view<SelectedComponent>().each()) {
				bool open = true;

				ImGui::SetNextWindowSize({ 200.f * scale, 200.f * scale }, ImGuiCond_FirstUseEver);

				const auto name = r.try_get<NameComponent>(selected);
				const auto windowTitle =
					name ?
					putils::string<64>("%s##[%d]", name->name.c_str(), selected) :
					putils::string<64>("[%d] Entity editor", selected);

				if (ImGui::Begin(windowTitle.c_str(), &open, ImGuiWindowFlags_NoSavedSettings))
					imguiHelper::editEntityAndModel({ r, selected });
				ImGui::End();

				if (!open)
					r.remove<SelectedComponent>(selected);
			}
		}
	};

	void addImGuiEntityEditorSystem(entt::registry & r) noexcept {
		const entt::handle e{ r, r.create() };
		e.emplace<ImGuiEntityEditorSystem>(e);
	}
}
