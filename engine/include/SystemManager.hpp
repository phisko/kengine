//
// Created by naliwe on 2/12/17.
//

#ifndef MEETABLE_CORE_SYSTEMMANAGER_HPP
# define MEETABLE_CORE_SYSTEMMANAGER_HPP

# include <map>
# include <memory>
# include "ISystem.hpp"

using system_map = std::multimap<ComponentMask, std::unique_ptr<ISystem>>;

class SystemManager
{
public:
    SystemManager(SystemManager const& o) = delete;
    SystemManager(SystemManager&& o) = delete;
    SystemManager& operator=(SystemManager const& o) = delete;

public:
    SystemManager();

public:
    template<typename T,
             typename = std::enable_if_t<std::is_base_of<ISystem, T>::value>>
    void registerSystem()
    {
        _sysMap.emplace(std::make_pair(T::Mask, std::make_unique<T>()));
    }

private:
    system_map _sysMap;
};

#endif //MEETABLE_CORE_SYSTEMMANAGER_HPP
