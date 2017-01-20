//
// Created by naliwe on 6/24/16.
//

#ifndef KENGINE_GAMEOBJECT_HPP
# define KENGINE_GAMEOBJECT_HPP

# include <string>
# include <map>
#include <algorithm>
# include "IComponent.hpp"
# include "Object.hpp"

class GameObject : public Object
{
public:
    GameObject(std::string const& name);
    GameObject(GameObject const& other);
    GameObject& operator=(GameObject other);
    GameObject& operator=(GameObject&& other);

    virtual ~GameObject();

public:
    friend void swap(GameObject& left, GameObject& right);

public:
    void attachComponent(IComponent *comp);
    void detachComponent(IComponent *comp);

    template<class CT,
             typename = typename std::enable_if<
                     std::is_base_of<IComponent, CT>::value
             >::type>
    CT& getComponent(std::string const& name)
    {
        if (_components.find(name) == _components.end())
            throw std::logic_error("Component " + name + " not found");

        return static_cast<CT&>(*_components[name]);
    };

    template<class CT,
             typename = typename std::enable_if<
                     std::is_base_of<IComponent, CT>::value
             >::type>
    CT& getComponent()
    {
        auto selected = std::find_if(_components.begin(), _components.end(),
                                     [](auto&& elem)
                                     {
                                         return (CT::Mask == elem.second->getMask());
                                     });

        if (selected == _components.end())
            throw std::logic_error("Could not find component with mask" + (uint8_t) CT::Mask);

        return static_cast<CT&>(*(selected->second));
    };

public:
    virtual std::string toString() const;

public:
    std::string const& get_name() const;

private:
    std::string                         _name;
    std::map<std::string, IComponent *> _components;
};

#endif //KENGINE_GAMEOBJECT_HPP
