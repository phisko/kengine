#pragma once

#include "Entity.hpp"

namespace kengine::resourceHelper {
	Entity::ID loadTexture(EntityManager & em, const char * file);
	Entity::ID loadTexture(EntityManager & em, void * data, size_t width, size_t height);
}