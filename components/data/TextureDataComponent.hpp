#pragma once

#ifndef KENGINE_TEXTURE_PATH_MAX_LENGTH
# define KENGINE_TEXTURE_PATH_MAX_LENGTH 256
#endif

#include "reflection.hpp"

namespace kengine {
	struct TextureDataComponent {
		void * data = nullptr;
		int width = 0;
		int height = 0;
		int components = 0;

		using FreeFunc = void(*)(void * data);
		FreeFunc free = nullptr;

		TextureDataComponent() = default;
		TextureDataComponent(const TextureDataComponent &) = delete;
		TextureDataComponent & operator=(const TextureDataComponent &) = delete;
		TextureDataComponent(TextureDataComponent && rhs)
			: data(rhs.data), width(rhs.width), height(rhs.height), components(rhs.components), free(rhs.free)
		{
			rhs.free = nullptr;
		}
		TextureDataComponent & operator=(TextureDataComponent && rhs) {
			width = rhs.width;
			height = rhs.height;
			components = rhs.components;
			std::swap(data, rhs.data);
			std::swap(free, rhs.free);
			return *this;
		}

		~TextureDataComponent() {
			if (free != nullptr)
				free(data);
		}
	};
}

#define refltype kengine::TextureDataComponent
putils_reflection_info{
	putils_reflection_class_name;
	putils_reflection_attributes(
		putils_reflection_attribute(data),
		putils_reflection_attribute(width),
		putils_reflection_attribute(height),
		putils_reflection_attribute(components)
	);
};
#undef refltype
