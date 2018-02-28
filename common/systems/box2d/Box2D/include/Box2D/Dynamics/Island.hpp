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

#ifndef B2_ISLAND_HPP
#define B2_ISLAND_HPP

#include <Box2D/Common/Math.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/TimeStep.hpp>

namespace b2
{

class Contact;
class Joint;
class StackAllocator;
class ContactListener;
struct ContactVelocityConstraint;
struct Profile;

/// This is an internal class.
class Island
{
public:
	Island(int32 bodyCapacity, int32 contactCapacity, int32 jointCapacity,
		   StackAllocator* allocator, ContactListener* listener);
	~Island();

	void Clear()
	{
		m_bodyCount = 0;
		m_contactCount = 0;
		m_jointCount = 0;
	}

	void Solve(Profile* profile, const TimeStep& step, const Vec2& gravity, bool allowSleep);

	void SolveTOI(const TimeStep& subStep, int32 toiIndexA, int32 toiIndexB);

	void Add(Body* body)
	{
		assert(m_bodyCount < m_bodyCapacity);
		body->m_islandIndex = m_bodyCount;
		m_bodies[m_bodyCount] = body;
		++m_bodyCount;
	}

	void Add(Contact* contact)
	{
		assert(m_contactCount < m_contactCapacity);
		m_contacts[m_contactCount++] = contact;
	}

	void Add(Joint* joint)
	{
		assert(m_jointCount < m_jointCapacity);
		m_joints[m_jointCount++] = joint;
	}

	void Report(const ContactVelocityConstraint* constraints);

	StackAllocator* m_allocator;
	ContactListener* m_listener;

	Body** m_bodies;
	Contact** m_contacts;
	Joint** m_joints;

	Position* m_positions;
	Velocity* m_velocities;

	int32 m_bodyCount;
	int32 m_jointCount;
	int32 m_contactCount;

	int32 m_bodyCapacity;
	int32 m_contactCapacity;
	int32 m_jointCapacity;
};

} // namespace b2

#endif // B2_ISLAND_HPP
