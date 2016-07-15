//
// Created by naliwe on 7/15/16.
//

#ifndef KENGINE_ISYSTEM_HPP
# define KENGINE_ISYSTEM_HPP

# include "Component.hpp"

class ISystem
{
public:
    virtual ~ISystem()
    { }

public:
    virtual void execute()                          = 0;
    virtual void registerComponent(Component& comp) = 0;
};


#endif //KENGINE_ISYSTEM_HPP
