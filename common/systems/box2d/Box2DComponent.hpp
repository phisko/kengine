#pragma once

#include "SerializableComponent.hpp"
#include "Box2D/Box2D.hpp"

namespace kengine {
    class Box2DComponent : public Component<Box2DComponent> {
    public:
        Box2DComponent(b2::Body * body = nullptr) : body(body) {}

    public:
        b2::Body * body;

    public:
        std::string toString() const noexcept final { return "{}"; }
    };
}
