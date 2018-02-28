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

#include <Box2D/Dynamics/Fixture.hpp>
#include <Box2D/Dynamics/Contacts/Contact.hpp>
#include <Box2D/Dynamics/World.hpp>
#include <Box2D/Collision/Shapes/CircleShape.hpp>
#include <Box2D/Collision/Shapes/EdgeShape.hpp>
#include <Box2D/Collision/Shapes/PolygonShape.hpp>
#include <Box2D/Collision/Shapes/ChainShape.hpp>
#include <Box2D/Collision/BroadPhase.hpp>
#include <Box2D/Collision/Collision.hpp>
#include <Box2D/Common/BlockAllocator.hpp>

namespace b2
{

Fixture::Fixture()
{
	m_userData = NULL;
	m_body = NULL;
	m_next = NULL;
	m_proxies = NULL;
	m_proxyCount = 0;
	m_shape = NULL;
	m_density = 0.0f;
}

void Fixture::Create(BlockAllocator* allocator, Body* body, const FixtureDef* def)
{
	m_userData = def->userData;
	m_friction = def->friction;
	m_restitution = def->restitution;

	m_body = body;
	m_next = NULL;

	m_filter = def->filter;

	m_isSensor = def->isSensor;

	m_shape = def->shape->Clone(allocator);

	// Reserve proxy space
	int32 childCount = m_shape->GetChildCount();
	m_proxies = (FixtureProxy*)allocator->Allocate(childCount * sizeof(FixtureProxy));
	for (int32 i = 0; i < childCount; ++i)
	{
		m_proxies[i].fixture = NULL;
		m_proxies[i].proxyId = BroadPhase::e_nullProxy;
	}
	m_proxyCount = 0;

	m_density = def->density;
}

void Fixture::Destroy(BlockAllocator* allocator)
{
	// The proxies must be destroyed before calling this.
	assert(m_proxyCount == 0);

	// Free the proxy array.
	int32 childCount = m_shape->GetChildCount();
	allocator->Free(m_proxies, childCount * sizeof(FixtureProxy));
	m_proxies = NULL;

	// Free the child shape.
	switch (m_shape->m_type)
	{
	case Shape::e_circle:
		{
			CircleShape* s = (CircleShape*)m_shape;
			s->~CircleShape();
			allocator->Free(s, sizeof(CircleShape));
		}
		break;

	case Shape::e_edge:
		{
			EdgeShape* s = (EdgeShape*)m_shape;
			s->~EdgeShape();
			allocator->Free(s, sizeof(EdgeShape));
		}
		break;

	case Shape::e_polygon:
		{
			PolygonShape* s = (PolygonShape*)m_shape;
			s->~PolygonShape();
			allocator->Free(s, sizeof(PolygonShape));
		}
		break;

	case Shape::e_chain:
		{
			ChainShape* s = (ChainShape*)m_shape;
			s->~ChainShape();
			allocator->Free(s, sizeof(ChainShape));
		}
		break;

	default:
		assert(false);
		break;
	}

	m_shape = NULL;
}

void Fixture::CreateProxies(BroadPhase* broadPhase, const Transform& xf)
{
	assert(m_proxyCount == 0);

	// Create proxies in the broad-phase.
	m_proxyCount = m_shape->GetChildCount();

	for (int32 i = 0; i < m_proxyCount; ++i)
	{
		FixtureProxy* proxy = m_proxies + i;
		m_shape->ComputeAABB(&proxy->aabb, xf, i);
		proxy->proxyId = broadPhase->CreateProxy(proxy->aabb, proxy);
		proxy->fixture = this;
		proxy->childIndex = i;
	}
}

void Fixture::DestroyProxies(BroadPhase* broadPhase)
{
	// Destroy proxies in the broad-phase.
	for (int32 i = 0; i < m_proxyCount; ++i)
	{
		FixtureProxy* proxy = m_proxies + i;
		broadPhase->DestroyProxy(proxy->proxyId);
		proxy->proxyId = BroadPhase::e_nullProxy;
	}

	m_proxyCount = 0;
}

void Fixture::Synchronize(BroadPhase* broadPhase, const Transform& transform1, const Transform& transform2)
{
	if (m_proxyCount == 0)
	{	
		return;
	}

	for (int32 i = 0; i < m_proxyCount; ++i)
	{
		FixtureProxy* proxy = m_proxies + i;

		// Compute an AABB that covers the swept shape (may miss some rotation effect).
		AABB aabb1, aabb2;
		m_shape->ComputeAABB(&aabb1, transform1, proxy->childIndex);
		m_shape->ComputeAABB(&aabb2, transform2, proxy->childIndex);
	
		proxy->aabb.Combine(aabb1, aabb2);

		Vec2 displacement = transform2.p - transform1.p;

		broadPhase->MoveProxy(proxy->proxyId, proxy->aabb, displacement);
	}
}

void Fixture::SetFilterData(const Filter& filter)
{
	m_filter = filter;

	Refilter();
}

void Fixture::Refilter()
{
	if (m_body == NULL)
	{
		return;
	}

	// Flag associated contacts for filtering.
	ContactEdge* edge = m_body->GetContactList();
	while (edge)
	{
		Contact* contact = edge->contact;
		Fixture* fixtureA = contact->GetFixtureA();
		Fixture* fixtureB = contact->GetFixtureB();
		if (fixtureA == this || fixtureB == this)
		{
			contact->FlagForFiltering();
		}

		edge = edge->next;
	}

	World* world = m_body->GetWorld();

	if (world == NULL)
	{
		return;
	}

	// Touch each proxy so that new pairs may be created
	BroadPhase* broadPhase = &world->m_contactManager.m_broadPhase;
	for (int32 i = 0; i < m_proxyCount; ++i)
	{
		broadPhase->TouchProxy(m_proxies[i].proxyId);
	}
}

void Fixture::SetSensor(bool sensor)
{
	if (sensor != m_isSensor)
	{
		m_body->SetAwake(true);
		m_isSensor = sensor;
	}
}

void Fixture::Dump(int32 bodyIndex)
{
	Log("    b2::FixtureDef fd;\n");
	Log("    fd.friction = %.15lef;\n", m_friction);
	Log("    fd.restitution = %.15lef;\n", m_restitution);
	Log("    fd.density = %.15lef;\n", m_density);
	Log("    fd.isSensor = bool(%d);\n", m_isSensor);
	Log("    fd.filter.categoryBits = uint16(%d);\n", m_filter.categoryBits);
	Log("    fd.filter.maskBits = uint16(%d);\n", m_filter.maskBits);
	Log("    fd.filter.groupIndex = int16(%d);\n", m_filter.groupIndex);

	switch (m_shape->m_type)
	{
	case Shape::e_circle:
		{
			CircleShape* s = (CircleShape*)m_shape;
			Log("    b2::CircleShape shape;\n");
			Log("    shape.m_radius = %.15lef;\n", s->m_radius);
			Log("    shape.m_p.Set(%.15lef, %.15lef);\n", s->m_p.x, s->m_p.y);
		}
		break;

	case Shape::e_edge:
		{
			EdgeShape* s = (EdgeShape*)m_shape;
			Log("    b2::EdgeShape shape;\n");
			Log("    shape.m_radius = %.15lef;\n", s->m_radius);
			Log("    shape.m_vertex0.Set(%.15lef, %.15lef);\n", s->m_vertex0.x, s->m_vertex0.y);
			Log("    shape.m_vertex1.Set(%.15lef, %.15lef);\n", s->m_vertex1.x, s->m_vertex1.y);
			Log("    shape.m_vertex2.Set(%.15lef, %.15lef);\n", s->m_vertex2.x, s->m_vertex2.y);
			Log("    shape.m_vertex3.Set(%.15lef, %.15lef);\n", s->m_vertex3.x, s->m_vertex3.y);
			Log("    shape.m_hasVertex0 = bool(%d);\n", s->m_hasVertex0);
			Log("    shape.m_hasVertex3 = bool(%d);\n", s->m_hasVertex3);
		}
		break;

	case Shape::e_polygon:
		{
			PolygonShape* s = (PolygonShape*)m_shape;
			Log("    b2::PolygonShape shape;\n");
			Log("    b2::Vec2 vs[%d];\n", maxPolygonVertices);
			for (int32 i = 0; i < s->m_count; ++i)
			{
				Log("    vs[%d].Set(%.15lef, %.15lef);\n", i, s->m_vertices[i].x, s->m_vertices[i].y);
			}
			Log("    shape.Set(vs, %d);\n", s->m_count);
		}
		break;

	case Shape::e_chain:
		{
			ChainShape* s = (ChainShape*)m_shape;
			Log("    b2::ChainShape shape;\n");
			Log("    b2::Vec2 vs[%d];\n", s->m_count);
			for (int32 i = 0; i < s->m_count; ++i)
			{
				Log("    vs[%d].Set(%.15lef, %.15lef);\n", i, s->m_vertices[i].x, s->m_vertices[i].y);
			}
			Log("    shape.CreateChain(vs, %d);\n", s->m_count);
			Log("    shape.m_prevVertex.Set(%.15lef, %.15lef);\n", s->m_prevVertex.x, s->m_prevVertex.y);
			Log("    shape.m_nextVertex.Set(%.15lef, %.15lef);\n", s->m_nextVertex.x, s->m_nextVertex.y);
			Log("    shape.m_hasPrevVertex = bool(%d);\n", s->m_hasPrevVertex);
			Log("    shape.m_hasNextVertex = bool(%d);\n", s->m_hasNextVertex);
		}
		break;

	default:
		return;
	}

	Log("\n");
	Log("    fd.shape = &shape;\n");
	Log("\n");
	Log("    bodies[%d]->CreateFixture(&fd);\n", bodyIndex);
}

} // namespace b2
