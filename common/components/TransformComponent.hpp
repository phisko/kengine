#pragma once

#include "SerializableComponent.hpp"
#include "Point.hpp"
#include "concat.hpp"

namespace kengine
{
    template<typename Precision, std::size_t Dimensions>
    class TransformComponent : public putils::Reflectible<TransformComponent<Precision, Dimensions>>,
                               public kengine::SerializableComponent<TransformComponent<Precision, Dimensions>>
    {
    public:
        TransformComponent(const putils::Point<Precision, 3> &pos = { 0, 0, 0 },
                           const putils::Point<Precision, 3> &size = { 1, 1, 1 })
                : boundingBox(pos, size)
        {}

        const std::string type = "transform";
        putils::Rect<Precision, Dimensions> boundingBox;
        Precision yaw = 0; // Radians

        /*
         * Reflectible
         */

    public:
        static const auto &get_attributes()
        {
            static const auto table = pmeta::make_table(
                    "type", &TransformComponent::type,
                    "boundingBox", &TransformComponent::boundingBox,
                    "yaw", &TransformComponent::yaw
            );
            return table;
        }

        static const auto &get_methods()
        {
            static const auto table = pmeta::make_table();
            return table;
        }

        static const auto &get_parents()
        {
            static const auto table = pmeta::make_table();
            return table;
        }
    };

    using TransformComponent2d = TransformComponent<double, 2>;
    using TransformComponent3d = TransformComponent<double, 3>;

    using TransformComponent2f = TransformComponent<float, 2>;
    using TransformComponent3f = TransformComponent<float, 3>;
};
