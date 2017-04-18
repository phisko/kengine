//
// Created by naliwe on 6/24/16.
//

#pragma once

#include "meta/type.hpp"
#include "Object.hpp"
#include "Module.hpp"

namespace kengine
{
    class IComponent : public virtual putils::BaseModule
    {
    public:
        virtual ~IComponent() = default;

        friend std::ostream &operator<<(std::ostream &s, const kengine::IComponent &obj)
        {
            s << obj.toString();
            return s;
        }

    private:
        virtual std::string toString() const = 0;

    public:
        virtual pmeta::type_index getType() const noexcept = 0;
    };
}
