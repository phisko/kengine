#pragma once

namespace kengine {
    class GameObject;

    namespace packets {
        struct RegisterGameObject {
            GameObject & go;
        };
    }
}