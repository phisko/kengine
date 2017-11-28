#pragma once

#include "GameObject.hpp"
#include "common/components/TransformComponent.hpp"
#include "common/components/PhysicsComponent.hpp"

namespace kengine {
    class KinematicObject : public GameObject, public putils::Reflectible<KinematicObject> {
    public:
        KinematicObject(std::string_view name, const putils::Rect3d & box = { { 0, 0, 0 },
                                                                              { 1, 1, 1 } },
                        bool solid = true, bool fixed = false)
                : GameObject(name) {
            attachComponent<kengine::TransformComponent3d>(box);
            attachComponent<kengine::PhysicsComponent>(solid, fixed);
        }

        static const auto get_class_name() { return pmeta_nameof(KinematicObject); }
    };
}
