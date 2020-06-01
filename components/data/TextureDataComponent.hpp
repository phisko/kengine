#pragma once

#ifndef KENGINE_TEXTURE_PATH_MAX_LENGTH
# define KENGINE_TEXTURE_PATH_MAX_LENGTH 256
#endif

namespace kengine {
	struct TextureDataComponent {
		void * data = nullptr;
		GLuint * textureID = nullptr;

		int width = 0;
		int height = 0;
		int components = 0;

		using FreeFunc = void(*)(void * data);
		FreeFunc free = nullptr;
	};
}
