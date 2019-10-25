#include "AnimationManager.hpp"
#include "EntityManager.hpp"

#include <OgreAnimationState.h>

#include "components/AnimationComponent.hpp"
#include "components/GraphicsComponent.hpp"
#include "components/GraphicsComponent.hpp"

#include "OgreObjectComponent.hpp"

struct OgreAnimationComponent {
	Ogre::AnimationState * state;
};

AnimationManager::AnimationManager(kengine::EntityManager & em) : _em(em) {
}

void AnimationManager::execute(float time) noexcept {
	for (const auto & [e, graphics, anim, object, comp] : _em.getEntities<kengine::GraphicsComponent, kengine::AnimationComponent, OgreObjectComponent, OgreAnimationComponent>()) {
		if (graphics.model == kengine::Entity::INVALID_ID)
			continue;

		auto & model = _em.getEntity(graphics.model);
		if (!model.has<kengine::AnimListComponent>()) {
			auto & animList = model.attach<kengine::AnimListComponent>();

			const auto anims = object.entity->getAllAnimationStates();
			if (anims == nullptr)
				continue;
			for (const auto & [name, state] : anims->getAnimationStates()) {
				kengine::AnimListComponent::Anim anim;
				anim.name = name.c_str();
				anim.totalTime = state->getLength();
				anim.ticksPerSecond = 0.f;
				animList.allAnims.push_back(anim);
			}
		}

		const auto & animList = model.attach<kengine::AnimListComponent>();
		const auto & currentAnim = animList.allAnims[anim.currentAnim];
		auto state = object.entity->getAnimationState(currentAnim.name.c_str());
		assert(state != nullptr);
		if (state != comp.state && comp.state != nullptr)
			comp.state->setEnabled(false);
		comp.state = state;

		comp.state->setEnabled(true);
		comp.state->setLoop(anim.loop);
		if (!comp.state->hasEnded()) {
			comp.state->addTime(time * anim.speed);
			anim.currentTime += time;
		}
	}
}

void AnimationManager::registerEntity(kengine::Entity & e) noexcept {
	if (!e.has<kengine::AnimationComponent>() || !e.has<kengine::GraphicsComponent>())
		return;
	e += OgreAnimationComponent{};
}
