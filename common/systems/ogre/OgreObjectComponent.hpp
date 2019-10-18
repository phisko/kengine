// Ogre dll warnings
#pragma warning(disable : 4251 4275) 

#pragma once

#include <OgreSceneNode.h>
#include <OgreEntity.h>

struct OgreObjectComponent {
	Ogre::SceneNode * node;
	Ogre::Entity * entity;
};