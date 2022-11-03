#include "pluginHelper.hpp"
#include "kengine.hpp"

// imgui
#include <imgui.h>

// kengine data
#include "data/ImGuiContextComponent.hpp"

// kengine helpers
#include "helpers/profilingHelper.hpp"

namespace kengine::pluginHelper {
    void initPlugin(void * state) noexcept {
		KENGINE_PROFILING_SCOPE;

        kengine::initPlugin(state);
        for (const auto & [e, context] : entities.with<kengine::ImGuiContextComponent>())
            ImGui::SetCurrentContext(context.context);
    }
}