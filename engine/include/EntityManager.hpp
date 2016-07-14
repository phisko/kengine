//
// Created by naliwe on 7/14/16.
//

#ifndef KENGINE_ENTITYMANAGER_HPP
# define KENGINE_ENTITYMANAGER_HPP

# include <string>
# include <map>
# include <memory>
# include <type_traits>
# include "GameObject.hpp"

class EntityManager
{
public:
    EntityManager();
    ~EntityManager();

public:
    EntityManager(EntityManager const& o) = delete;
    EntityManager(EntityManager&& o)      = delete;

public:
    template<class GO, class ...Args,
             typename = typename std::enable_if<
                     std::is_base_of<GameObject, GO>::value
             >::type>
    GO& createEntity(std::string const& name, Args&& ... params)
    {
        auto selected = _entities.find(name);

        if (selected != _entities.end())
            throw std::logic_error("GameObject with name " + name + " already exists");

        _entities.emplace(name, std::make_unique<GO>(name, std::forward(params)...));

        return *_entities[name];
    }

private:
    std::map<std::string, std::unique_ptr<GameObject>> _entities;
};


#endif //KENGINE_ENTITYMANAGER_HPP
