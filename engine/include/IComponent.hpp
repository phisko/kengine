//
// Created by naliwe on 6/24/16.
//

#ifndef KENGINE_ICOMPONENT_HPP
# define KENGINE_ICOMPONENT_HPP

# include "KTypes.hpp"
# include "Object.hpp"
# include "Module.hpp"

namespace kengine
{
    class IComponent : public Object, public putils::Module
    {
    public:
        virtual ComponentMask getMask() const noexcept = 0;

        virtual bool isUnique() const noexcept = 0;

        virtual std::string const &getName() const noexcept = 0;

        virtual ~IComponent() {}
    };
}

#endif //KENGINE_ICOMPONENT_HPP
