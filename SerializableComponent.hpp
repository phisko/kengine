#pragma once

#include "Component.hpp"
#include "putils/Serializable.hpp"

namespace kengine
{
    template<typename CRTP>
    class SerializableComponent : public Component<CRTP>, public putils::Serializable<CRTP>
    {
    public:
        template<typename ...MemberPairs>
        SerializableComponent(MemberPairs &&...pairs)
                : putils::Serializable<CRTP>(FWD(pairs)...)
        {}

        std::string toString() const noexcept override
        {
            std::stringstream s;
            this->serialize(s);
            return s.str();
        }
    };
}
