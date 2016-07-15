//
// Created by naliwe on 7/14/16.
//

#ifndef KENGINE_ENTITYMANAGER_HPP
# define KENGINE_ENTITYMANAGER_HPP

# include <string>
# include <map>
# include <memory>
# include <type_traits>
# include <iostream>
# include "GameObject.hpp"
# include "Component.hpp"
# include "ComponentFactory.hpp"

class EntityManager
{
public:
    EntityManager();
    ~EntityManager();

public:
    EntityManager(EntityManager const& o) = delete;
    EntityManager(EntityManager&& o)      = delete;
    EntityManager& operator=(EntityManager const& o) = delete;

public:
    template<class GO, class ...Args,
             typename = typename std::enable_if<
                     std::is_base_of<GameObject, GO>::value
             >::type>
    constexpr GO& createEntity(std::string const& name, Args&& ... params) noexcept
    {
        _entities.emplace(name, std::make_unique<GO>(name, std::forward(params)...));

        return *_entities[name];
    }

    template<class CT, class ... Args>
    constexpr CT& attachComponent(GameObject const& parent, std::string const& name, Args... params) noexcept
    {
        auto str = hashCompName(parent.get_name(), name);

        _components.emplace(str,
                            ComponentFactory::createComponent<CT>(name, std::forward(params)...));

        return *_components[str];
    };

private:
    std::string hashCompName(std::string const& pName, std::string const& cName)
    {
        return pName + "--" + cName;
    }

private:
    std::map<std::string, std::unique_ptr<GameObject>> _entities;
    std::map<std::string, std::unique_ptr<Component>>  _components;
    std::map<std::string, std::string>                 _compHierarchy;
};


#endif //KENGINE_ENTITYMANAGER_HPP
