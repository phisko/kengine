//
// Created by naliwe on 6/24/16.
//

#pragma once

#include "Object.hpp"
#include "Module.hpp"

namespace kengine
{
    class IComponent : public Object, public virtual putils::BaseModule
    {
    public:
        virtual std::size_t getMask() const noexcept = 0;

        virtual bool isUnique() const noexcept = 0;

        virtual std::string const &getName() const noexcept = 0;

        virtual ~IComponent() {}
    };
}
