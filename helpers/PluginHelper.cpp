#include "PluginHelper.hpp"
#include "EntityManager.hpp"

namespace kengine::PluginHelper {
    void initPlugin(EntityManager & em) {
        detail::components = &em._getComponentMap();
    }
}