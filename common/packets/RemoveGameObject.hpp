#pragma once

namespace kengine {
    class GameObject;

    namespace packets {
        struct RemoveGameObject {
            GameObject & go;
        };
    }
}
