#pragma once

#include "function.hpp"

namespace kengine {
	class Entity;
	using EntityCreator = void(Entity &);

	template<size_t Size>
	using EntityCreatorFunctor = putils::function<EntityCreator, Size>;
}