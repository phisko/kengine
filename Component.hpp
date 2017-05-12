//
// Created by naliwe on 6/24/16.
//

#pragma once

#include <string>
#include "IComponent.hpp"
#include "PooledObject.hpp"

namespace kengine
{
    template<typename CRTP, typename ...DataPackets>
    class Component : public IComponent, public putils::Module<CRTP, DataPackets...>, public putils::PooledObject<CRTP>
    {
    public:
        pmeta::type_index getType() const noexcept override { return pmeta::type<CRTP>::index; }
    };
}
