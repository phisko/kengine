#pragma once

#include <string>
#include "SerializableComponent.hpp"
#include "Point.hpp"

namespace kengine {
    class GraphicsComponent : public kengine::SerializableComponent<GraphicsComponent>,
                          public putils::Reflectible<GraphicsComponent> {
    public:
        GraphicsComponent(const std::string & appearance = "") {
			if (!appearance.empty())
				addLayer("main", appearance);
        }

        const std::string type = pmeta_nameof(GraphicsComponent);
        bool repeated = false;

		struct Layer {
			std::string name;
			std::string appearance;
			putils::Rect3d boundingBox = { {}, { 1, 1, 1 } };
			double yaw = 0;

			pmeta_get_class_name(GraphicsComponent::Layer);
			pmeta_get_attributes(
				pmeta_reflectible_attribute(&Layer::name),
				pmeta_reflectible_attribute(&Layer::appearance),
				pmeta_reflectible_attribute(&Layer::boundingBox),
				pmeta_reflectible_attribute(&Layer::yaw)
			);
			pmeta_get_methods();
			pmeta_get_parents();
		};

		std::vector<Layer> layers;

		Layer & addLayer(const std::string & name, const std::string & appearance) {
			layers.push_back({ name, appearance });
			return layers.back();
		}

		void removeLayer(const std::string & name) {
			const auto it = std::find_if(layers.begin(), layers.end(), [&name](auto && layer) { return layer.name == name; });
			if (it != layers.end())
				layers.erase(it);
		}

		bool hasLayer(const std::string & name) const {
			return std::find_if(layers.begin(), layers.end(), [&name](auto && layer) { return layer.name == name; }) != layers.end();
		}

		Layer & getLayer(const std::string & name) {
			const auto it = std::find_if(layers.begin(), layers.end(), [&name](auto && layer) { return layer.name == name; });
			return *it;
		}

        /*
         * Reflectible
         */

    public:
        pmeta_get_class_name(GraphicsComponent);
        pmeta_get_attributes(
                pmeta_reflectible_attribute(&GraphicsComponent::type),
                pmeta_reflectible_attribute(&GraphicsComponent::repeated),
                pmeta_reflectible_attribute(&GraphicsComponent::layers)
        );
		pmeta_get_methods(
			pmeta_reflectible_attribute(&GraphicsComponent::addLayer),
			pmeta_reflectible_attribute(&GraphicsComponent::removeLayer)
		);
    };
}
