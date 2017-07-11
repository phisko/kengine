//
// Created by naliwe on 7/15/16.
//

#pragma once

#include <vector>
#include "meta/type.hpp"
#include "Component.hpp"
#include "GameObject.hpp"
#include "Module.hpp"

namespace kengine
{
    class ISystem : public virtual putils::BaseModule
    {
    protected:
        ISystem() = default;

    public:
        virtual ~ISystem() = default;

    public:
        virtual void execute() {}
        virtual void registerGameObject(GameObject& go) {}
        virtual void removeGameObject(GameObject& go) {}
        virtual pmeta::type_index getType() const noexcept = 0;
    };
}
