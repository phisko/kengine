#pragma once

#include "Component.hpp"
#include "reflection/Serializable.hpp"

namespace kengine {
    template<typename CRTP>
    class SerializableComponent : public Component<CRTP>,
                                  public putils::Serializable<CRTP> {
    public:
        template<typename ...MemberPairs>
        SerializableComponent(MemberPairs && ...pairs)
                : putils::Serializable<CRTP>(FWD(pairs)...) {}

        SerializableComponent() = default;

        std::string toString() const noexcept final {
            std::stringstream s;
            this->serialize(s);
            return s.str();
        }

    public:
        pmeta_get_class_name(SerializableComponent);
        pmeta_get_attributes();
        pmeta_get_methods();
        pmeta_get_parents();
    };
}
