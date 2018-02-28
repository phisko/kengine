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

#include <Box2D/Dynamics/Contacts/PolygonAndCircleContact.hpp>
#include <Box2D/Common/BlockAllocator.hpp>
#include <Box2D/Dynamics/Fixture.hpp>

#include <new>

namespace b2
{

Contact* PolygonAndCircleContact::Create(Fixture* fixtureA, int32, Fixture* fixtureB, int32, BlockAllocator* allocator)
{
	void* mem = allocator->Allocate(sizeof(PolygonAndCircleContact));
	return new (mem) PolygonAndCircleContact(fixtureA, fixtureB);
}

void PolygonAndCircleContact::Destroy(Contact* contact, BlockAllocator* allocator)
{
	((PolygonAndCircleContact*)contact)->~PolygonAndCircleContact();
	allocator->Free(contact, sizeof(PolygonAndCircleContact));
}

PolygonAndCircleContact::PolygonAndCircleContact(Fixture* fixtureA, Fixture* fixtureB)
: Contact(fixtureA, 0, fixtureB, 0)
{
	assert(m_fixtureA->GetType() == Shape::e_polygon);
	assert(m_fixtureB->GetType() == Shape::e_circle);
}

void PolygonAndCircleContact::Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB)
{
	CollidePolygonAndCircle(manifold,
							(PolygonShape*)m_fixtureA->GetShape(), xfA,
							(CircleShape*)m_fixtureB->GetShape(), xfB);
}

} // namespace b2
