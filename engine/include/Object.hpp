//
// Created by naliwe on 7/5/16.
//

#ifndef KENGINE_OBJECT_HPP
# define KENGINE_OBJECT_HPP

# include <string>

class Object
{
public:
    virtual std::string const& toString() const = 0;

    virtual ~Object() { }
};

#endif //KENGINE_OBJECT_HPP
