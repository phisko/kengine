#include "pluginHelper.hpp"
#include "kengine.hpp"

// imgui
#include <imgui.h>

// kengine data
#include "data/ImGuiContextComponent.hpp"

namespace kengine::pluginHelper {
    void initPlugin(void * state) noexcept {
        kengine::initPlugin(state);

        for (const auto & [e, context] : entities.with<kengine::ImGuiContextComponent>())
            ImGui::SetCurrentContext(context.context);
    }
}