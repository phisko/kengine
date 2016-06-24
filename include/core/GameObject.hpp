//
// Created by naliwe on 6/24/16.
//

#ifndef KENGINE_GAMEOBJECT_HPP
# define KENGINE_GAMEOBJECT_HPP

# include <string>

class GameObject
{
public:
    GameObject(std::string const& name);

private:
    std::string _name;
};

#endif //KENGINE_GAMEOBJECT_HPP
