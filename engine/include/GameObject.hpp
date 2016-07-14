//
// Created by naliwe on 6/24/16.
//

#ifndef KENGINE_GAMEOBJECT_HPP
# define KENGINE_GAMEOBJECT_HPP

# include <string>
# include <map>
# include "IComponent.hpp"
# include "Object.hpp"

class GameObject : public Object
{
public:
    GameObject(std::string const& name);
    GameObject(GameObject const& other);
    GameObject(GameObject&& other);
    GameObject& operator=(GameObject other);
    GameObject& operator=(GameObject&& other);

    virtual ~GameObject();

public:
    friend void swap(GameObject& left, GameObject& right);

public:
    void attachComponent(IComponent const& comp);
    void attachComponent(IComponent&& comp);

public:
    virtual std::string toString() const;

private:
    std::string                              _name;
    std::map<std::string const, IComponent&> _components;
};

#endif //KENGINE_GAMEOBJECT_HPP
