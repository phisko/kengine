#pragma once

#include "BaseFunction.hpp"

namespace kengine {
	class Entity;

	namespace functions {
		struct DisplayImGui : BaseFunction<void(const kengine::Entity &)> {};
		struct EditImGui : BaseFunction<void(kengine::Entity &)> {};
	}
}