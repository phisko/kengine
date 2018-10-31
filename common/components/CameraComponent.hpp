#pragma once

#include "SerializableComponent.hpp"
#include "Point.hpp"
#include "concat.hpp"

namespace kengine {
    template<typename Precision, std::size_t Dimensions>
    class CameraComponent : public putils::Reflectible<CameraComponent<Precision, Dimensions>>,
                            public kengine::SerializableComponent<CameraComponent<Precision, Dimensions>> {
    public:
        CameraComponent(const putils::Point<Precision, Dimensions> & pos = { 0, 0 },
                        const putils::Point<Precision, Dimensions> & size = { 1, 1 })
                : frustrum(pos, size) {}

        CameraComponent(const putils::Rect<Precision, Dimensions> & rect)
                : frustrum(rect) {}

        const std::string type = pmeta_nameof(CameraComponent);
        putils::Rect<Precision, Dimensions> frustrum;
        Precision pitch = 0; // Radians
        Precision yaw = 0; // Radians

		putils::Point2f getCoordinatesFromScreen(const putils::Point2f & screenCoordinates, const putils::Point2f & screenSize) const noexcept {
			auto ret = screenCoordinates;

			ret.x /= screenSize.x;
			ret.y /= screenSize.y;

			ret.x *= frustrum.size.x;
			ret.y *= frustrum.size.z;

			const auto & offset = frustrum.topLeft;
			ret.x += frustrum.topLeft.x;
			ret.y += frustrum.topLeft.z;

			return ret;
		}

        /*
         * Reflectible
         */

    public:
        pmeta_get_class_name(CameraComponent);
        pmeta_get_attributes(
                pmeta_reflectible_attribute(&CameraComponent::type),
                pmeta_reflectible_attribute(&CameraComponent::frustrum),
                pmeta_reflectible_attribute(&CameraComponent::pitch),
                pmeta_reflectible_attribute(&CameraComponent::yaw)
        );
        pmeta_get_methods();
        pmeta_get_parents();
    };

    using CameraComponent2i = CameraComponent<int, 2>;
    using CameraComponent3i = CameraComponent<int, 3>;

    using CameraComponent2d = CameraComponent<double, 2>;
    using CameraComponent3d = CameraComponent<double, 3>;

    using CameraComponent2f = CameraComponent<float, 2>;
    using CameraComponent3f = CameraComponent<float, 3>;
};
