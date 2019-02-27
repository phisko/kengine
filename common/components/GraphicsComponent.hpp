#pragma once

#ifndef KENGINE_GRAPHICS_STRING_MAX_LENGTH
# define KENGINE_GRAPHICS_STRING_MAX_LENGTH 64
#endif

#ifndef KENGINE_GRAPHICS_MAX_LAYERS
# define KENGINE_GRAPHICS_MAX_LAYERS 8
#endif

#define PUTILS_STRING_LENGTH KENGINE_GRAPHICS_STRING_MAX_LENGTH
#include "string.hpp"
#undef PUTILS_STRING_LENGTH

#define PUTILS_VECTOR_CLASS GraphicsComponentLayer
#define PUTILS_VECTOR_LENGTH KENGINE_GRAPHICS_MAX_LAYERS
#include "vector.hpp"
#undef PUTILS_VECTOR_CLASS
#undef PUTILS_VECTOR_LENGTH

#include "Point.hpp"

namespace kengine {
    class GraphicsComponent : public putils::Reflectible<GraphicsComponent> {
    public:
		using string = putils::string<KENGINE_GRAPHICS_STRING_MAX_LENGTH>;

    public:
        GraphicsComponent(const char * appearance = nullptr) {
			if (appearance != nullptr)
				addLayer("main", appearance);
        }

        bool repeated = false;

		struct Layer : public putils::Reflectible<Layer> {
			string name;
			string appearance;
			putils::Rect3f boundingBox = { {}, { 1, 1, 1 } };
			float yaw = 0;
			bool mirrored = false;

			pmeta_get_class_name(GraphicsComponentLayer);
			pmeta_get_attributes(
				pmeta_reflectible_attribute(&Layer::name),
				pmeta_reflectible_attribute(&Layer::appearance),
				pmeta_reflectible_attribute(&Layer::boundingBox),
				pmeta_reflectible_attribute(&Layer::yaw)
			);
			pmeta_get_methods();
			pmeta_get_parents();
		};
		using layer_vector = putils::vector<Layer, KENGINE_GRAPHICS_MAX_LAYERS>;
		layer_vector layers;

		Layer & addLayer(const char * name, const char * appearance) {
			auto & ret = layers.emplace_back();
			ret.name = name;
			ret.appearance = appearance;
			return layers.back();
		}

		void removeLayer(const char * name) {
			const auto it = std::find_if(layers.begin(), layers.end(), [&name](auto && layer) { return layer.name == name; });
			if (it != layers.end())
				layers.erase(it);
		}

		bool hasLayer(const char * name) const {
			return std::find_if(layers.begin(), layers.end(), [&name](auto && layer) { return layer.name == name; }) != layers.end();
		}

		Layer & getLayer(const char * name) {
			const auto it = std::find_if(layers.begin(), layers.end(), [&name](auto && layer) { return layer.name == name; });
			return *it;
		}

        /*
         * Reflectible
         */

    public:
        pmeta_get_class_name(GraphicsComponent);
        pmeta_get_attributes(
                pmeta_reflectible_attribute(&GraphicsComponent::repeated),
                pmeta_reflectible_attribute(&GraphicsComponent::layers)
        );
		pmeta_get_methods(
			pmeta_reflectible_attribute(&GraphicsComponent::addLayer),
			pmeta_reflectible_attribute(&GraphicsComponent::removeLayer),
			pmeta_reflectible_attribute(&GraphicsComponent::getLayer),
			pmeta_reflectible_attribute(&GraphicsComponent::hasLayer)
		);
		pmeta_get_parents();
    };
}
