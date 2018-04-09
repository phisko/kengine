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

#include <Box2D/Dynamics/Contacts/Contact.hpp>
#include <Box2D/Dynamics/Contacts/CircleContact.hpp>
#include <Box2D/Dynamics/Contacts/PolygonAndCircleContact.hpp>
#include <Box2D/Dynamics/Contacts/PolygonContact.hpp>
#include <Box2D/Dynamics/Contacts/EdgeAndCircleContact.hpp>
#include <Box2D/Dynamics/Contacts/EdgeAndPolygonContact.hpp>
#include <Box2D/Dynamics/Contacts/ChainAndCircleContact.hpp>
#include <Box2D/Dynamics/Contacts/ChainAndPolygonContact.hpp>
#include <Box2D/Dynamics/Contacts/ContactSolver.hpp>

#include <Box2D/Collision/Collision.hpp>
#include <Box2D/Collision/TimeOfImpact.hpp>
#include <Box2D/Collision/Shapes/Shape.hpp>
#include <Box2D/Common/BlockAllocator.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <Box2D/Dynamics/World.hpp>

namespace b2
{

ContactRegister Contact::s_registers[Shape::e_typeCount][Shape::e_typeCount];
bool Contact::s_initialized = false;

void Contact::InitializeRegisters()
{
	AddType(CircleContact::Create, CircleContact::Destroy, Shape::e_circle, Shape::e_circle);
	AddType(PolygonAndCircleContact::Create, PolygonAndCircleContact::Destroy, Shape::e_polygon, Shape::e_circle);
	AddType(PolygonContact::Create, PolygonContact::Destroy, Shape::e_polygon, Shape::e_polygon);
	AddType(EdgeAndCircleContact::Create, EdgeAndCircleContact::Destroy, Shape::e_edge, Shape::e_circle);
	AddType(EdgeAndPolygonContact::Create, EdgeAndPolygonContact::Destroy, Shape::e_edge, Shape::e_polygon);
	AddType(ChainAndCircleContact::Create, ChainAndCircleContact::Destroy, Shape::e_chain, Shape::e_circle);
	AddType(ChainAndPolygonContact::Create, ChainAndPolygonContact::Destroy, Shape::e_chain, Shape::e_polygon);
}

void Contact::AddType(ContactCreateFcn* createFcn, ContactDestroyFcn* destoryFcn,
					  Shape::Type type1, Shape::Type type2)
{
	assert(0 <= type1 && type1 < Shape::e_typeCount);
	assert(0 <= type2 && type2 < Shape::e_typeCount);
	
	s_registers[type1][type2].createFcn = createFcn;
	s_registers[type1][type2].destroyFcn = destoryFcn;
	s_registers[type1][type2].primary = true;

	if (type1 != type2)
	{
		s_registers[type2][type1].createFcn = createFcn;
		s_registers[type2][type1].destroyFcn = destoryFcn;
		s_registers[type2][type1].primary = false;
	}
}

Contact* Contact::Create(Fixture* fixtureA, int32 indexA, Fixture* fixtureB, int32 indexB, BlockAllocator* allocator)
{
	if (s_initialized == false)
	{
		InitializeRegisters();
		s_initialized = true;
	}

	Shape::Type type1 = fixtureA->GetType();
	Shape::Type type2 = fixtureB->GetType();

	assert(0 <= type1 && type1 < Shape::e_typeCount);
	assert(0 <= type2 && type2 < Shape::e_typeCount);
	
	ContactCreateFcn* createFcn = s_registers[type1][type2].createFcn;
	if (createFcn)
	{
		if (s_registers[type1][type2].primary)
		{
			return createFcn(fixtureA, indexA, fixtureB, indexB, allocator);
		}
		else
		{
			return createFcn(fixtureB, indexB, fixtureA, indexA, allocator);
		}
	}
	else
	{
		return NULL;
	}
}

void Contact::Destroy(Contact* contact, BlockAllocator* allocator)
{
	assert(s_initialized == true);

	Fixture* fixtureA = contact->m_fixtureA;
	Fixture* fixtureB = contact->m_fixtureB;

	if (contact->m_manifold.pointCount > 0 &&
		fixtureA->IsSensor() == false &&
		fixtureB->IsSensor() == false)
	{
		fixtureA->GetBody()->SetAwake(true);
		fixtureB->GetBody()->SetAwake(true);
	}

	Shape::Type typeA = fixtureA->GetType();
	Shape::Type typeB = fixtureB->GetType();

	assert(0 <= typeA && typeB < Shape::e_typeCount);
	assert(0 <= typeA && typeB < Shape::e_typeCount);

	ContactDestroyFcn* destroyFcn = s_registers[typeA][typeB].destroyFcn;
	destroyFcn(contact, allocator);
}

Contact::Contact(Fixture* fA, int32 indexA, Fixture* fB, int32 indexB)
{
	m_flags = e_enabledFlag;

	m_fixtureA = fA;
	m_fixtureB = fB;

	m_indexA = indexA;
	m_indexB = indexB;

	m_manifold.pointCount = 0;

	m_prev = NULL;
	m_next = NULL;

	m_nodeA.contact = NULL;
	m_nodeA.prev = NULL;
	m_nodeA.next = NULL;
	m_nodeA.other = NULL;

	m_nodeB.contact = NULL;
	m_nodeB.prev = NULL;
	m_nodeB.next = NULL;
	m_nodeB.other = NULL;

	m_toiCount = 0;

	m_friction = MixFriction(m_fixtureA->m_friction, m_fixtureB->m_friction);
	m_restitution = MixRestitution(m_fixtureA->m_restitution, m_fixtureB->m_restitution);

	m_tangentSpeed = 0.0f;
}

// Update the contact manifold and touching status.
// Note: do not assume the fixture AABBs are overlapping or are valid.
void Contact::Update(ContactListener* listener)
{
	Manifold oldManifold = m_manifold;

	// Re-enable this contact.
	m_flags |= e_enabledFlag;

	bool touching = false;
	bool wasTouching = (m_flags & e_touchingFlag) == e_touchingFlag;

	bool sensorA = m_fixtureA->IsSensor();
	bool sensorB = m_fixtureB->IsSensor();
	bool sensor = sensorA || sensorB;

	Body* bodyA = m_fixtureA->GetBody();
	Body* bodyB = m_fixtureB->GetBody();
	const Transform& xfA = bodyA->GetTransform();
	const Transform& xfB = bodyB->GetTransform();

	// Is this contact a sensor?
	if (sensor)
	{
		const Shape* shapeA = m_fixtureA->GetShape();
		const Shape* shapeB = m_fixtureB->GetShape();
		touching = TestOverlap(shapeA, m_indexA, shapeB, m_indexB, xfA, xfB);

		// Sensors don't generate manifolds.
		m_manifold.pointCount = 0;
	}
	else
	{
		Evaluate(&m_manifold, xfA, xfB);
		touching = m_manifold.pointCount > 0;

		// Match old contact ids to new contact ids and copy the
		// stored impulses to warm start the solver.
		for (int32 i = 0; i < m_manifold.pointCount; ++i)
		{
			ManifoldPoint* mp2 = m_manifold.points + i;
			mp2->normalImpulse = 0.0f;
			mp2->tangentImpulse = 0.0f;
			ContactID id2 = mp2->id;

			for (int32 j = 0; j < oldManifold.pointCount; ++j)
			{
				ManifoldPoint* mp1 = oldManifold.points + j;

				if (mp1->id.key == id2.key)
				{
					mp2->normalImpulse = mp1->normalImpulse;
					mp2->tangentImpulse = mp1->tangentImpulse;
					break;
				}
			}
		}

		if (touching != wasTouching)
		{
			bodyA->SetAwake(true);
			bodyB->SetAwake(true);
		}
	}

	if (touching)
	{
		m_flags |= e_touchingFlag;
	}
	else
	{
		m_flags &= ~e_touchingFlag;
	}

	if (wasTouching == false && touching == true && listener)
	{
		listener->BeginContact(this);
	}

	if (wasTouching == true && touching == false && listener)
	{
		listener->EndContact(this);
	}

	if (sensor == false && touching && listener)
	{
		listener->PreSolve(this, &oldManifold);
	}
}

} // namespace b2
