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
        putils_reflection_class_name(TransformComponent);
        putils_reflection_attributes(
                putils_reflection_attribute(&TransformComponent::boundingBox),
                putils_reflection_attribute(&TransformComponent::pitch),
                putils_reflection_attribute(&TransformComponent::yaw),
                putils_reflection_attribute(&TransformComponent::roll)
        );
    };

    using TransformComponent2i = TransformComponent<int, 2>;
    using TransformComponent3i = TransformComponent<int, 3>;

    using TransformComponent2d = TransformComponent<double, 2>;
    using TransformComponent3d = TransformComponent<double, 3>;

    using TransformComponent2f = TransformComponent<float, 2>;
    using TransformComponent3f = TransformComponent<float, 3>;
};
