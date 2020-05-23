#pragma once

namespace kengine {
	class EntityManager;
	class Entity;

	namespace imguiHelper {
		void displayEntity(EntityManager & em, const Entity & e);
		void editEntity(EntityManager & em, Entity & e);
	}
}