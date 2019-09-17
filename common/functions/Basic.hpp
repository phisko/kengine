#pragma once

#include "BaseFunction.hpp"

namespace kengine {
	class Entity;

	namespace functions {
		struct Has : BaseFunction<bool(const Entity &)> {};
		struct Attach : BaseFunction<void(Entity &)> {};
		struct Detach : BaseFunction<void(Entity &)> {};
	}
}