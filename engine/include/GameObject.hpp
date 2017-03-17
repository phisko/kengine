//
// Created by naliwe on 6/24/16.
//

#ifndef KENGINE_GAMEOBJECT_HPP
# define KENGINE_GAMEOBJECT_HPP

# include <string>
# include <unordered_map>
# include <algorithm>
# include "IComponent.hpp"
# include "Object.hpp"
# include "Mediator.hpp"

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

            return static_cast<CT *>(*it->second);
        };

        template<class CT,
                typename = typename std::enable_if_t<
                        std::is_base_of<IComponent, CT>::value>>
        CT &getComponent() const
        {
            auto selected = std::find_if(_components.begin(), _components.end(),
                    [](auto &&elem)
                    {
                        return (CT::Mask == elem.second->getMask());
                    });

            if (selected == _components.end())
                throw std::logic_error("Could not find component with mask" + (uint8_t) CT::Mask);

            return static_cast<CT &>(*(selected->second));
        };

    public:
        std::string toString() const override;

    public:
        std::string const &getName() const override { return _name; }
        ComponentMask getMask() const { return _mask; }

    private:
        std::string _name;
        std::unordered_map<std::string, IComponent *> _components;
        ComponentMask _mask = ComponentMask::Default;
    };
}

#endif //KENGINE_GAMEOBJECT_HPP
