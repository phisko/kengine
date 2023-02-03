#pragma once

#ifndef KENGINE_JSON_SCENE_LOADER_STRING_MAX_LENGTH
#define KENGINE_JSON_SCENE_LOADER_STRING_MAX_LENGTH 64
#endif

// stl
#include <future>

// putils
#include "putils/string.hpp"

namespace kengine::data {
	//! putils reflect all
	//! used_types: [refltype::string]
	struct json_scene_loader {
		static constexpr char string_name[] = "json_scene_loader_string";
		using string = putils::string<KENGINE_JSON_SCENE_LOADER_STRING_MAX_LENGTH, string_name>;

		string temporary_scene;
		string model_directory;
		string scene;
	};
}

#include "json_scene_loader.rpp"