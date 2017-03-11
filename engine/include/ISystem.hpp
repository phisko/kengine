//
// Created by naliwe on 7/15/16.
//

#ifndef KENGINE_ISYSTEM_HPP
# define KENGINE_ISYSTEM_HPP

# include "Component.hpp"
# include "GameObject.hpp"

class ISystem
{
public:
    static const ComponentMask Mask;

public:
    virtual ~ISystem()
    {}

public:
    virtual void execute()                          = 0;
    virtual void registerGameObject(GameObject& go) = 0;
    virtual void removeGameObject(GameObject& go) = 0;
};


#endif //KENGINE_ISYSTEM_HPP
