#include "PluginHelper.hpp"
#include "EntityManager.hpp"

#include "data/ImGuiContextComponent.hpp"
#include "imgui.h"

namespace kengine::pluginHelper {
    void initPlugin(EntityManager & em) {
        detail::components = &em._getComponentMap();

        for (const auto & [e, context] : em.getEntities<kengine::ImGuiContextComponent>())
            ImGui::SetCurrentContext(context.context);
    }
}