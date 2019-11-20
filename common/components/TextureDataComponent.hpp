#pragma once

#ifndef KENGINE_TEXTURE_PATH_MAX_LENGTH
# define KENGINE_TEXTURE_PATH_MAX_LENGTH 256
#endif

namespace kengine {
	struct TextureDataComponent {
		void * data;
		GLuint * textureID;

		int width;
		int height;
		int components;

		using FreeFunc = void(*)(void * data);
		FreeFunc free;
	};
}
