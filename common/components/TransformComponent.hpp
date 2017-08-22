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
        TransformComponent(const putils::Point<Precision, Dimensions> &pos = { 0, 0 },
                           const putils::Point<Precision, Dimensions> &size = { 1, 1 })
                : boundingBox(pos, size)
        {}

        TransformComponent(const putils::Rect<Precision, Dimensions> &rect)
                : boundingBox(rect)
        {}

        const std::string type = pmeta_nameof(TransformComponent);
        putils::Rect<Precision, Dimensions> boundingBox;
        Precision pitch = 0; // Radians
        Precision yaw = 0; // Radians

        /*
         * Reflectible
         */

    public:
        static const auto get_class_name() { return pmeta_nameof(TransformComponent); }

        static const auto &get_attributes()
        {
            static const auto table = pmeta::make_table(
                    pmeta_reflectible_attribute(&TransformComponent::type),
                    pmeta_reflectible_attribute(&TransformComponent::boundingBox),
                    pmeta_reflectible_attribute(&TransformComponent::pitch),
                    pmeta_reflectible_attribute(&TransformComponent::yaw)
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

    using TransformComponent2i = TransformComponent<int, 2>;
    using TransformComponent3i = TransformComponent<int, 3>;

    using TransformComponent2d = TransformComponent<double, 2>;
    using TransformComponent3d = TransformComponent<double, 3>;

    using TransformComponent2f = TransformComponent<float, 2>;
    using TransformComponent3f = TransformComponent<float, 3>;
};
