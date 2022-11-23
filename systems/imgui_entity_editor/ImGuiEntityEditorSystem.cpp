#include "ImGuiEntityEditorSystem.hpp"

// entt
#include <entt/entity/handle.hpp>
#include <entt/entity/registry.hpp>

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
		static void init(entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, Log, "Init", "ImGuiEntityEditorSystem");

			_r = &r;

			const auto e = r.create();
			r.emplace<functions::Execute>(e, execute);

			r.emplace<NameComponent>(e, "Entity editor");
			auto & tool = r.emplace<ImGuiToolComponent>(e, true);
			_enabled = &tool.enabled;
		}

		static void execute(float deltaTime) noexcept {
			KENGINE_PROFILING_SCOPE;

			if (!*_enabled)
				return;

			kengine_log(*_r, Verbose, "Execute", "ImGuiEntityEditorSystem");
			const auto scale = imguiHelper::getScale(*_r);
			for (auto [selected] : _r->view<SelectedComponent>().each()) {
				bool open = true;

				ImGui::SetNextWindowSize({ 200.f * scale, 200.f * scale }, ImGuiCond_FirstUseEver);

				const auto name = _r->try_get<NameComponent>(selected);
				const auto windowTitle =
					name ?
					putils::string<64>("%s##[%d]", name->name.c_str(), selected) :
					putils::string<64>("[%d] Entity editor", selected);

				if (ImGui::Begin(windowTitle.c_str(), &open, ImGuiWindowFlags_NoSavedSettings))
					imguiHelper::editEntityAndModel({ *_r, selected });
				ImGui::End();

				if (!open)
					_r->remove<SelectedComponent>(selected);
			}
		}

		static inline bool * _enabled;
		static inline entt::registry * _r;
	};

	void ImGuiEntityEditorSystem(entt::registry & r) noexcept {
		ImGuiEntityEditorSystem::init(r);
	}
}
