#pragma once

#include "Manager.hpp"

class AnimationManager : public Manager {
public:
	AnimationManager(kengine::EntityManager & em);

	void execute(float time) noexcept override;
	void registerEntity(kengine::Entity & e) noexcept override;

private:
	kengine::EntityManager & _em;
};