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
        void detachComponent(const IComponent *comp);

        template<typename CT, typename ...Args, typename = std::enable_if_t<std::is_base_of<kengine::IComponent, CT>::value>>
        void attachComponent(Args &&...args)
        {
            attachComponent(new CT(std::forward<Args>(args)...));
        };

    public:
        template<class CT,
                typename = typename std::enable_if_t<
                        std::is_base_of<IComponent, CT>::value>>
        CT &getComponent() const
        {
            const auto it = _components.find(CT::Type);
            if (it == _components.end())
                throw std::out_of_range("Could not find component with provided type");

            return static_cast<CT &>(*(it->second));
        };

    public:
        template<typename CT, typename = std::enable_if_t<std::is_base_of<IComponent, CT>::value>>
        bool hasComponent() const noexcept { return _components.find(CT::Type) != _components.end(); }

    public:
        std::string toString() const override;

    public:
        const std::string &getName() const { return _name; }
        const std::vector<pmeta::type_index> &getTypes() const { return _types; }

    private:
        std::string _name;
        std::unordered_map<pmeta::type_index, IComponent *> _components;
        std::vector<pmeta::type_index > _types;
    };
}
