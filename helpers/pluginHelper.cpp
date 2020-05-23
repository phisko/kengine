#include "PluginHelper.hpp"
#include "EntityManager.hpp"

namespace kengine::pluginHelper {
    void initPlugin(EntityManager & em) {
        detail::components = &em._getComponentMap();
    }
}