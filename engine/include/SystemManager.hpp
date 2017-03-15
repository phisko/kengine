//
// Created by naliwe on 2/12/17.
//

#ifndef CORE_SYSTEMMANAGER_HPP
# define CORE_SYSTEMMANAGER_HPP

# include <vector>
# include <memory>
# include "ISystem.hpp"
# include "GameObject.hpp"

class SystemManager
{
public:
    SystemManager(SystemManager const& o) = delete;
    SystemManager& operator=(SystemManager const& o) = delete;

public:
    SystemManager() = default;
    ~SystemManager() = default;

public:
    void execute()
    {
        for (auto &category : _systems)
            for (auto &s : category.second)
                s->execute();
    }

public:
    void registerGameObject(GameObject &gameObject)
    {
        for (auto &category : _systems)
            if (category.first & gameObject.getMask())
                for (auto &s : category.second)
                    s->registerGameObject(gameObject);
    }

    void removeGameObject(GameObject &gameObject)
    {
        for (auto &category : _systems)
            if (category.first & gameObject.getMask())
                for (auto &s : category.second)
                    s->removeGameObject(gameObject);
    }

public:
    template<typename T, typename ...Args,
             typename = std::enable_if_t<std::is_base_of<ISystem, T>::value>>
    T &registerSystem(Args &&...args)
    {
        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        auto &ret = *system;
        auto &category = _systems[T::Mask];
        category.emplace_back(std::move(system));
        return ret;
    }

private:
    using Category = std::vector<std::unique_ptr<ISystem>>;
    std::unordered_map<ComponentMask, Category> _systems;
};

#endif //MEETABLE_CORE_SYSTEMMANAGER_HPP
