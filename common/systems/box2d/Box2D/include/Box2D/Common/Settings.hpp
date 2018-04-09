/*
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef B2_SETTINGS_HPP
#define B2_SETTINGS_HPP

#include <cstddef>
#include <cassert>
#include <cfloat>

#define B2_NOT_USED(x) ((void)(x))

namespace b2
{

typedef signed char	int8;
typedef signed short int16;
typedef signed int int32;
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef float float32;
typedef double float64;

const float32 maxFloat  = FLT_MAX;
const float32 epsilon   = FLT_EPSILON;
const float32 pi        = 3.14159265359f;

/// @file
/// Global tuning constants based on meters-kilograms-seconds (MKS) units.
///

// Collision

/// The maximum number of contact points between two convex shapes. Do
/// not change this value.
const int32 maxManifoldPoints	= 2;

/// The maximum number of vertices on a convex polygon. You cannot increase
/// this too much because b2::BlockAllocator has a maximum object size.
const int32 maxPolygonVertices	= 8;

/// This is used to fatten AABBs in the dynamic tree. This allows proxies
/// to move by a small amount without triggering a tree adjustment.
/// This is in meters.
const float32 aabbExtension		= 0.1f;

/// This is used to fatten AABBs in the dynamic tree. This is used to predict
/// the future position based on the current displacement.
/// This is a dimensionless multiplier.
const float32 aabbMultiplier	= 2.0f;

/// A small length used as a collision and constraint tolerance. Usually it is
/// chosen to be numerically significant, but visually insignificant.
const float32 linearSlop		= 0.005f;

/// A small angle used as a collision and constraint tolerance. Usually it is
/// chosen to be numerically significant, but visually insignificant.
const float32 angularSlop		= (2.0f / 180.0f * pi);

/// The radius of the polygon/edge shape skin. This should not be modified. Making
/// this smaller means polygons will have an insufficient buffer for continuous collision.
/// Making it larger may create artifacts for vertex collision.
const float32 polygonRadius		= (2.0f * linearSlop);

/// Maximum number of sub-steps per contact in continuous physics simulation.
const int32 maxSubSteps			= 8;


// Dynamics

/// Maximum number of contacts to be handled to solve a TOI impact.
const int32 maxTOIContacts			= 32;

/// A velocity threshold for elastic collisions. Any collision with a relative linear
/// velocity below this threshold will be treated as inelastic.
const float32 velocityThreshold		= 1.0f;

/// The maximum linear position correction used when solving constraints. This helps to
/// prevent overshoot.
const float32 maxLinearCorrection	= 0.2f;

/// The maximum angular position correction used when solving constraints. This helps to
/// prevent overshoot.
const float32 maxAngularCorrection	= (8.0f / 180.0f * pi);

/// The maximum linear velocity of a body. This limit is very large and is used
/// to prevent numerical problems. You shouldn't need to adjust this.
const float32 maxTranslation		= 2.0f;
const float32 maxTranslationSquared	= (maxTranslation * maxTranslation);

/// The maximum angular velocity of a body. This limit is very large and is used
/// to prevent numerical problems. You shouldn't need to adjust this.
const float32 maxRotation			= (0.5f * pi);
const float32 maxRotationSquared	= (maxRotation * maxRotation);

/// This scale factor controls how fast overlap is resolved. Ideally this would be 1 so
/// that overlap is removed in one time step. However using values close to 1 often lead
/// to overshoot.
const float32 baumgarte				= 0.2f;
const float32 toiBaugarte			= 0.75f;


// Sleep

/// The time that a body must be still before it will go to sleep.
const float32 timeToSleep			= 0.5f;

/// A body cannot sleep if its linear velocity is above this tolerance.
const float32 linearSleepTolerance	= 0.01f;

/// A body cannot sleep if its angular velocity is above this tolerance.
const float32 angularSleepTolerance	= (2.0f / 180.0f * pi);


// Memory Allocation

/// Implement this function to use your own memory allocator.
void* Alloc(int32 size);

/// If you implement b2::Alloc, you should also implement this function.
void Free(void* mem);

/// Logging function.
void Log(const char* string, ...);

/// Version numbering scheme.
/// See http://en.wikipedia.org/wiki/Software_versioning
struct Version
{
	int32 major;		///< significant changes
	int32 minor;		///< incremental changes
	int32 revision;		///< bug fixes
};

/// Current version.
extern Version version;

} // namespace b2

#endif // B2_SETTINGS_HPP
