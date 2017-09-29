#pragma once

#include "SerializableComponent.hpp"
#include "Point.hpp"
#include "concat.hpp"

namespace kengine
{
    template<typename Precision, std::size_t Dimensions>
    class CameraComponent : public putils::Reflectible<CameraComponent<Precision, Dimensions>>,
                               public kengine::SerializableComponent<CameraComponent<Precision, Dimensions>>
    {
    public:
        CameraComponent(const putils::Point<Precision, Dimensions> &pos = { 0, 0 },
                           const putils::Point<Precision, Dimensions> &size = { 1, 1 })
                : frustrum(pos, size)
        {}

        CameraComponent(const putils::Rect<Precision, Dimensions> &rect)
                : frustrum(rect)
        {}

        const std::string type = pmeta_nameof(CameraComponent);
        putils::Rect<Precision, Dimensions> frustrum;
        Precision pitch = 0; // Radians
        Precision yaw = 0; // Radians

        /*
         * Reflectible
         */

    public:
        static const auto get_class_name() { return pmeta_nameof(CameraComponent); }

        static const auto &get_attributes()
        {
            static const auto table = pmeta::make_table(
                    pmeta_reflectible_attribute(&CameraComponent::type),
                    pmeta_reflectible_attribute(&CameraComponent::frustrum),
                    pmeta_reflectible_attribute(&CameraComponent::pitch),
                    pmeta_reflectible_attribute(&CameraComponent::yaw)
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

    using CameraComponent2i = CameraComponent<int, 2>;
    using CameraComponent3i = CameraComponent<int, 3>;

    using CameraComponent2d = CameraComponent<double, 2>;
    using CameraComponent3d = CameraComponent<double, 3>;

    using CameraComponent2f = CameraComponent<float, 2>;
    using CameraComponent3f = CameraComponent<float, 3>;
};
