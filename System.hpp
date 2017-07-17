#pragma once

#include <cstddef>
#include "ISystem.hpp"

namespace kengine
{
    template<typename CRTP, typename ...DataPackets>
    class System : public ISystem, public putils::Module<CRTP, DataPackets...>
    {
    protected:
        using ISystem::ISystem;

    public:
        pmeta::type_index getType() const noexcept final
        {
            static_assert(std::is_base_of<System, CRTP>::value,
                          "System's first template parameter should be inheriting class");
            return pmeta::type<CRTP>::index;
        }
    };
}