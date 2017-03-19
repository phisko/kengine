//
// Created by naliwe on 7/5/16.
//

#ifndef KENGINE_OBJECT_HPP
# define KENGINE_OBJECT_HPP

# include <string>

namespace kengine
{
    class Object
    {
    public:
        virtual std::string toString() const = 0;

        virtual std::string const &getName() const = 0;

        virtual ~Object() {}
    };
}

#endif //KENGINE_OBJECT_HPP
