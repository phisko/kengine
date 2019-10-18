#pragma once

#include <OgreVector.h>
#include "Point.hpp"

static Ogre::Vector3 convert(const putils::Point3f & p) { return { p.x, p.y, p.z }; }
static putils::Point3f convert(const Ogre::Vector3 & p) { return { p.x, p.y, p.z }; }

static Ogre::ColourValue convert(const putils::NormalizedColor & color) { return Ogre::ColourValue{ color.r, color.g, color.b, color.a }; }
static putils::NormalizedColor convert(const Ogre::ColourValue & color) { return { color.r, color.g, color.b, color.a }; }
