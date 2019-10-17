#pragma once

#include <OgreVector.h>
#include "Point.hpp"

static Ogre::Vector3 convert(const putils::Point3f & p) { return { p.x, p.y, p.z }; }
static putils::Point3f convert(const Ogre::Vector3 & p) { return { p.x, p.y, p.z }; }
