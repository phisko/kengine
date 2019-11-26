#pragma once

#include "BaseFunction.hpp"

namespace kengine {
	class Entity;

	namespace functions {
		struct MatchString : BaseFunction<bool(const Entity &, const char *)> {};
	}
}