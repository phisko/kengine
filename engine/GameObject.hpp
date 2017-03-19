//
// Created by naliwe on 6/24/16.
//

#pragma once

#include <string>
#include <unordered_map>
#include <algorithm>
#include "IComponent.hpp"
#include "Object.hpp"
#include "Mediator.hpp"

namespace kengine
{
    class GameObject : public Object, public putils::Mediator
    {
    public:
        GameObject(std::string const &name);
        GameObject(GameObject &&other) = default;
        GameObject &operator=(GameObject &&other) = default;

        virtual ~GameObject() = default;

    protected:
        friend class EntityManager;
        void attachComponent(IComponent *comp);
        void detachComponent(IComponent *comp);

        template<typename CT, typename ...Args, typename = std::enable_if_t<std::is_base_of<kengine::IComponent, CT>::value>>
        void attachComponent(Args &&...args)
        {
            attachComponent(new CT(std::forward<Args>(args)...));
        };

    public:
        template<class CT,
                typename = typename std::enable_if<
                        std::is_base_of<IComponent, CT>::value
                >::type>
        CT &getComponent(std::string const &name) const
        {
            auto it = _components.find(name);
            if (it == _components.end())
                throw std::logic_error("Component " + name + " not found");

            return static_cast<CT &>(*it->second);
        };

        template<class CT,
                typename = typename std::enable_if_t<
                        std::is_base_of<IComponent, CT>::value>>
        CT &getComponent() const
        {
            auto selected = std::find_if(_components.begin(), _components.end(),
                    [](auto &&elem)
                    {
                        return (CT::Type == elem.second->getType());
                    });

            if (selected == _components.end())
                throw std::logic_error("Could not find component with provided type");

            return static_cast<CT &>(*(selected->second));
        };

    public:
        std::string toString() const override;

    public:
        std::string const &getName() const override { return _name; }
        const std::vector<pmeta::type_index> &getTypes() const { return _types; }

    private:
        std::string _name;
        std::unordered_map<std::string, IComponent *> _components;
        std::vector<pmeta::type_index > _types;
    };
}
