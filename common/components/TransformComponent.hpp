#pragma once

#include "reflection.hpp"
#include "Point.hpp"

namespace kengine {
    template<typename Precision, std::size_t Dimensions>
    class TransformComponent {
    public:
        TransformComponent(const putils::Point<Precision, Dimensions> & pos = { 0, 0 },
                           const putils::Point<Precision, Dimensions> & size = { 1, 1 })
                : boundingBox(pos, size) {
            if constexpr (Dimensions == 3)
            if (size == putils::Point3f{ 1, 1, 0 })
                boundingBox.size.z = 1;
        }

        TransformComponent(const putils::Rect<Precision, Dimensions> & rect)
                : boundingBox(rect) {}

        putils::Rect<Precision, Dimensions> boundingBox;
        Precision pitch = 0; // Radians
        Precision yaw = 0; // Radians
		Precision roll = 0; // Radians

        /*
         * Reflectible
         */

    public:
        pmeta_get_class_name(TransformComponent);
        pmeta_get_attributes(
                pmeta_reflectible_attribute(&TransformComponent::boundingBox),
                pmeta_reflectible_attribute(&TransformComponent::pitch),
                pmeta_reflectible_attribute(&TransformComponent::yaw),
                pmeta_reflectible_attribute(&TransformComponent::roll)
        );
		pmeta_get_methods();
		pmeta_get_parents();
    };

    using TransformComponent2i = TransformComponent<int, 2>;
    using TransformComponent3i = TransformComponent<int, 3>;

    using TransformComponent2d = TransformComponent<double, 2>;
    using TransformComponent3d = TransformComponent<double, 3>;

    using TransformComponent2f = TransformComponent<float, 2>;
    using TransformComponent3f = TransformComponent<float, 3>;
};
