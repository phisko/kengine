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
        virtual void execute() = 0;
        virtual void registerGameObject(GameObject& go) = 0;
        virtual void removeGameObject(GameObject& go) = 0;
        virtual pmeta::type_index getCompType() const noexcept = 0;
    };
}
