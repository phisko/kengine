//
// Created by naliwe on 2/12/17.
//

#ifndef MEETABLE_CORE_SYSTEMMANAGER_HPP
# define MEETABLE_CORE_SYSTEMMANAGER_HPP

# include <unordered_map>
# include <memory>
# include "ISystem.hpp"
# include "GameObject.hpp"

class SystemManager
{
private:
    using system_map = std::unordered_multimap<ComponentMask, std::unique_ptr<ISystem>>;

public:
    SystemManager(SystemManager const& o) = delete;
    SystemManager(SystemManager&& o) = delete;
    SystemManager& operator=(SystemManager const& o) = delete;

public:
    SystemManager();

public:
    void execute()
    {
        for (auto &p : _sysMap)
            p.second->execute();
    }

public:
    void registerGameObject(GameObject &gameObject)
    {
        for (auto &p : _sysMap)
            if ((unsigned char)p.first & (unsigned char)gameObject.getMask())
                p.second->registerGameObject(gameObject);
    }
    void removeGameObject(GameObject &gameObject)
    {
        for (auto &p : _sysMap)
            if ((unsigned char)p.first & (unsigned char)gameObject.getMask())
                p.second->removeGameObject(gameObject);
    }

public:
    template<typename T, typename ...Args,
             typename = std::enable_if_t<std::is_base_of<ISystem, T>::value>>
    void registerSystem(Args &&...args)
    {
        _sysMap.emplace(std::make_pair(T::Mask, std::make_unique<T>(std::forward<Args>(args)...)));
    }

private:
    system_map _sysMap;
};

#endif //MEETABLE_CORE_SYSTEMMANAGER_HPP
