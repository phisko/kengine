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

#include <Box2D/Dynamics/Contacts/CircleContact.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <Box2D/Dynamics/WorldCallbacks.hpp>
#include <Box2D/Common/BlockAllocator.hpp>
#include <Box2D/Collision/TimeOfImpact.hpp>

#include <new>

namespace b2
{

Contact* CircleContact::Create(Fixture* fixtureA, int32, Fixture* fixtureB, int32, BlockAllocator* allocator)
{
	void* mem = allocator->Allocate(sizeof(CircleContact));
	return new (mem) CircleContact(fixtureA, fixtureB);
}

void CircleContact::Destroy(Contact* contact, BlockAllocator* allocator)
{
	((CircleContact*)contact)->~CircleContact();
	allocator->Free(contact, sizeof(CircleContact));
}

CircleContact::CircleContact(Fixture* fixtureA, Fixture* fixtureB)
	: Contact(fixtureA, 0, fixtureB, 0)
{
	assert(m_fixtureA->GetType() == Shape::e_circle);
	assert(m_fixtureB->GetType() == Shape::e_circle);
}

void CircleContact::Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB)
{
	CollideCircles(manifold,
				   (CircleShape*)m_fixtureA->GetShape(), xfA,
				   (CircleShape*)m_fixtureB->GetShape(), xfB);
}

} // namespace b2
