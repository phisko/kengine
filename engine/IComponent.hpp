//
// Created by naliwe on 6/24/16.
//

#pragma once

#include "meta/type.hpp"
#include "Object.hpp"
#include "Module.hpp"

namespace kengine
{
    class IComponent : public Object, public virtual putils::BaseModule
    {
    public:
        virtual ~IComponent() = default;

    public:
        virtual pmeta::type_index getType() const noexcept = 0;
    };
}
