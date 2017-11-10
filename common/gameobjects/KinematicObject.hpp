#pragma once

#include "GameObject.hpp"
#include "common/components/PhysicsComponent.hpp"

namespace kengine
{
    class KinematicObject : public GameObject, public putils::Reflectible<KinematicObject>
    {
    public:
        KinematicObject(std::string_view name, bool solid = true, bool fixed = false)
                : GameObject(name)
        {
            attachComponent<kengine::TransformComponent3d>();
            attachComponent<kengine::PhysicsComponent>(solid, fixed);
        }

        static const auto get_class_name() { return pmeta_nameof(KinematicObject); }
    };
}
