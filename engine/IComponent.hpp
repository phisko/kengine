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
        virtual pmeta::type_index getType() const noexcept = 0;

        virtual bool isUnique() const noexcept = 0;

        virtual std::string const &getName() const noexcept = 0;

        virtual ~IComponent() {}
    };
}