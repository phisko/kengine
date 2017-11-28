#pragma once

#include "Component.hpp"
#include "reflection/Serializable.hpp"

namespace kengine {
    template<typename CRTP, typename ...DataPackets>
    class SerializableComponent : public Component<CRTP, DataPackets...>,
                                  public putils::Serializable<CRTP, false> {
    public:
        template<typename ...MemberPairs>
        SerializableComponent(MemberPairs && ...pairs)
                : putils::Serializable<CRTP, false>(FWD(pairs)...) {}

        SerializableComponent() = default;

        std::string toString() const noexcept override {
            std::stringstream s;
            this->serialize(s);
            return s.str();
        }

    public:
        static const auto get_class_name() { return pmeta_nameof(SerializableComponent); }

        static const auto & get_attributes() {
            static const auto table = pmeta::make_table();
            return table;
        }

        static const auto & get_methods() {
            static const auto table = pmeta::make_table();
            return table;
        }

        static const auto & get_parents() {
            static const auto table = pmeta::make_table();
            return table;
        }
    };
}
