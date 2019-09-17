#pragma once

#include "BaseFunction.hpp"
#include "json.hpp"

namespace kengine {
	class Entity;

	namespace functions {
		struct LoadFromJSON : BaseFunction<void(const putils::json &, kengine::Entity &)> {};
	}
}