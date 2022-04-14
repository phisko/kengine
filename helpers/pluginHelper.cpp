#include "pluginHelper.hpp"
#include "kengine.hpp"

#include "data/ImGuiContextComponent.hpp"
#include "imgui.h"

namespace kengine::pluginHelper {
    void initPlugin(void * state) noexcept {
        kengine::initPlugin(state);

        for (const auto & [e, context] : entities.with<kengine::ImGuiContextComponent>())
            ImGui::SetCurrentContext(context.context);
    }
}