#pragma once

#include <string>
#include <unordered_map>
#include <algorithm>
#include <memory>

#include "IComponent.hpp"
#include "Mediator.hpp"

#include "Serializable.hpp"

#include "concat.hpp"
#include "json.hpp"
#include "to_string.hpp"
#include "fwd.hpp"

namespace kengine
{
    class GameObject : public putils::Mediator, public putils::Serializable<GameObject, false>
    {
    public:
        GameObject(std::string const &name) :
                Serializable(
                        std::make_pair("name", &GameObject::_name),
                        std::make_pair("components", &GameObject::_components)
                ),
                _name(name)
        {}

        GameObject(GameObject &&other) = default;
        GameObject &operator=(GameObject &&other) = default;
        ~GameObject() = default;

    protected:
        friend class EntityManager;

        template<typename CT, typename = std::enable_if_t<std::is_base_of<IComponent, CT>::value>>
        CT &attachComponent(std::unique_ptr<CT> &&comp)
        {
            auto &ret = *comp;

            this->addModule(comp.get());
            const auto type = comp->getType();
            _components.emplace(type, std::move(comp));
            _types.push_back(type);

            return ret;
        }

        void detachComponent(const IComponent &comp);

        template<typename CT, typename ...Args, typename = std::enable_if_t<std::is_base_of<kengine::IComponent, CT>::value>>
        CT &attachComponent(Args &&...args)
        {
            auto comp = std::make_unique<CT>(FWD(args)...);

            auto &ret = *comp;
            addModule(comp.get());
            const auto type = pmeta::type<CT>::index;
            _components.emplace(type, std::move(comp));
            _types.push_back(type);

            return ret;
        };

    public:
        template<class CT,
                typename = typename std::enable_if_t<
                        std::is_base_of<IComponent, CT>::value>>
        CT &getComponent()
        {
            return static_cast<CT &>(*_components.at(pmeta::type<CT>::index));
        };

        template<class CT,
                typename = typename std::enable_if_t<
                        std::is_base_of<IComponent, CT>::value>>
        const CT &getComponent() const
        {
            return static_cast<const CT &>(*_components.at(pmeta::type<CT>::index));
        };

    public:
        template<typename CT, typename = std::enable_if_t<std::is_base_of<IComponent, CT>::value>>
        bool hasComponent() const noexcept { return _components.find(pmeta::type<CT>::index) != _components.end(); }

    public:
        const std::string &getName() const { return _name; }
        const std::vector<pmeta::type_index> &getTypes() const { return _types; }

    private:
        std::string _name;
        std::unordered_map<pmeta::type_index, std::unique_ptr<IComponent>> _components;
        std::vector<pmeta::type_index > _types;
    };
}
