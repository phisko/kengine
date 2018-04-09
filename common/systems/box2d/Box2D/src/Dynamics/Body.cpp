/*
* Copyright (c) 2006-2007 Erin Catto http://www.box2d.org
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

#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <Box2D/Dynamics/World.hpp>
#include <Box2D/Dynamics/Contacts/Contact.hpp>
#include <Box2D/Dynamics/Joints/Joint.hpp>

namespace b2
{

Body::Body(const BodyDef* bd, World* world)
{
	assert(bd->position.IsValid());
	assert(bd->linearVelocity.IsValid());
	assert(IsValid(bd->angle));
	assert(IsValid(bd->angularVelocity));
	assert(IsValid(bd->angularDamping) && bd->angularDamping >= 0.0f);
	assert(IsValid(bd->linearDamping) && bd->linearDamping >= 0.0f);

	m_flags = 0;

	if (bd->bullet)
	{
		m_flags |= e_bulletFlag;
	}
	if (bd->fixedRotation)
	{
		m_flags |= e_fixedRotationFlag;
	}
	if (bd->allowSleep)
	{
		m_flags |= e_autoSleepFlag;
	}
	if (bd->awake)
	{
		m_flags |= e_awakeFlag;
	}
	if (bd->active)
	{
		m_flags |= e_activeFlag;
	}

	m_world = world;

	m_xf.p = bd->position;
	m_xf.q.Set(bd->angle);

	m_sweep.localCenter.SetZero();
	m_sweep.c0 = m_xf.p;
	m_sweep.c = m_xf.p;
	m_sweep.a0 = bd->angle;
	m_sweep.a = bd->angle;
	m_sweep.alpha0 = 0.0f;

	m_jointList = NULL;
	m_contactList = NULL;
	m_prev = NULL;
	m_next = NULL;

	m_linearVelocity = bd->linearVelocity;
	m_angularVelocity = bd->angularVelocity;

	m_linearDamping = bd->linearDamping;
	m_angularDamping = bd->angularDamping;
	m_gravityScale = bd->gravityScale;

	m_force.SetZero();
	m_torque = 0.0f;

	m_sleepTime = 0.0f;

	m_type = bd->type;

	if (m_type == dynamicBody)
	{
		m_mass = 1.0f;
		m_invMass = 1.0f;
	}
	else
	{
		m_mass = 0.0f;
		m_invMass = 0.0f;
	}

	m_I = 0.0f;
	m_invI = 0.0f;

	m_userData = bd->userData;

	m_fixtureList = NULL;
	m_fixtureCount = 0;
}

Body::~Body()
{
	// shapes and joints are destroyed in b2::World::Destroy
}

void Body::SetType(BodyType type)
{
	assert(m_world->IsLocked() == false);
	if (m_world->IsLocked() == true)
	{
		return;
	}

	if (m_type == type)
	{
		return;
	}

	m_type = type;

	ResetMassData();

	if (m_type == staticBody)
	{
		m_linearVelocity.SetZero();
		m_angularVelocity = 0.0f;
		m_sweep.a0 = m_sweep.a;
		m_sweep.c0 = m_sweep.c;
		SynchronizeFixtures();
	}

	SetAwake(true);

	m_force.SetZero();
	m_torque = 0.0f;

	// Delete the attached contacts.
	ContactEdge* ce = m_contactList;
	while (ce)
	{
		ContactEdge* ce0 = ce;
		ce = ce->next;
		m_world->m_contactManager.Destroy(ce0->contact);
	}
	m_contactList = NULL;

	// Touch the proxies so that new contacts will be created (when appropriate)
	BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
	for (Fixture* f = m_fixtureList; f; f = f->m_next)
	{
		int32 proxyCount = f->m_proxyCount;
		for (int32 i = 0; i < proxyCount; ++i)
		{
			broadPhase->TouchProxy(f->m_proxies[i].proxyId);
		}
	}
}

Fixture* Body::CreateFixture(const FixtureDef* def)
{
	assert(m_world->IsLocked() == false);
	if (m_world->IsLocked() == true)
	{
		return NULL;
	}

	BlockAllocator* allocator = &m_world->m_blockAllocator;

	void* memory = allocator->Allocate(sizeof(Fixture));
	Fixture* fixture = new (memory) Fixture;
	fixture->Create(allocator, this, def);

	if (m_flags & e_activeFlag)
	{
		BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
		fixture->CreateProxies(broadPhase, m_xf);
	}

	fixture->m_next = m_fixtureList;
	m_fixtureList = fixture;
	++m_fixtureCount;

	fixture->m_body = this;

	// Adjust mass properties if needed.
	if (fixture->m_density > 0.0f)
	{
		ResetMassData();
	}

	// Let the world know we have a new fixture. This will cause new contacts
	// to be created at the beginning of the next time step.
	m_world->m_flags |= World::e_newFixture;

	return fixture;
}

Fixture* Body::CreateFixture(const Shape* shape, float32 density)
{
	FixtureDef def;
	def.shape = shape;
	def.density = density;

	return CreateFixture(&def);
}

void Body::DestroyFixture(Fixture* fixture)
{
	assert(m_world->IsLocked() == false);
	if (m_world->IsLocked() == true)
	{
		return;
	}

	assert(fixture->m_body == this);

	// Remove the fixture from this body's singly linked list.
	assert(m_fixtureCount > 0);
	Fixture** node = &m_fixtureList;
	bool found = false;
	while (*node != NULL)
	{
		if (*node == fixture)
		{
			*node = fixture->m_next;
			found = true;
			break;
		}

		node = &(*node)->m_next;
	}

	// You tried to remove a shape that is not attached to this body.
	assert(found);

	// Destroy any contacts associated with the fixture.
	ContactEdge* edge = m_contactList;
	while (edge)
	{
		Contact* c = edge->contact;
		edge = edge->next;

		Fixture* fixtureA = c->GetFixtureA();
		Fixture* fixtureB = c->GetFixtureB();

		if (fixture == fixtureA || fixture == fixtureB)
		{
			// This destroys the contact and removes it from
			// this body's contact list.
			m_world->m_contactManager.Destroy(c);
		}
	}

	BlockAllocator* allocator = &m_world->m_blockAllocator;

	if (m_flags & e_activeFlag)
	{
		BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
		fixture->DestroyProxies(broadPhase);
	}

	fixture->Destroy(allocator);
	fixture->m_body = NULL;
	fixture->m_next = NULL;
	fixture->~Fixture();
	allocator->Free(fixture, sizeof(Fixture));

	--m_fixtureCount;

	// Reset the mass data.
	ResetMassData();
}

void Body::ResetMassData()
{
	// Compute mass data from shapes. Each shape has its own density.
	m_mass = 0.0f;
	m_invMass = 0.0f;
	m_I = 0.0f;
	m_invI = 0.0f;
	m_sweep.localCenter.SetZero();

	// Static and kinematic bodies have zero mass.
	if (m_type == staticBody || m_type == kinematicBody)
	{
		m_sweep.c0 = m_xf.p;
		m_sweep.c = m_xf.p;
		m_sweep.a0 = m_sweep.a;
		return;
	}

	assert(m_type == dynamicBody);

	// Accumulate mass over all fixtures.
	Vec2 localCenter = Vec2_zero;
	for (Fixture* f = m_fixtureList; f; f = f->m_next)
	{
		if (f->m_density == 0.0f)
		{
			continue;
		}

		MassData massData;
		f->GetMassData(&massData);
		m_mass += massData.mass;
		localCenter += massData.mass * massData.center;
		m_I += massData.I;
	}

	// Compute center of mass.
	if (m_mass > 0.0f)
	{
		m_invMass = 1.0f / m_mass;
		localCenter *= m_invMass;
	}
	else
	{
		// Force all dynamic bodies to have a positive mass.
		m_mass = 1.0f;
		m_invMass = 1.0f;
	}

	if (m_I > 0.0f && (m_flags & e_fixedRotationFlag) == 0)
	{
		// Center the inertia about the center of mass.
		m_I -= m_mass * Dot(localCenter, localCenter);
		assert(m_I > 0.0f);
		m_invI = 1.0f / m_I;

	}
	else
	{
		m_I = 0.0f;
		m_invI = 0.0f;
	}

	// Move center of mass.
	Vec2 oldCenter = m_sweep.c;
	m_sweep.localCenter = localCenter;
	m_sweep.c0 = m_sweep.c = Mul(m_xf, m_sweep.localCenter);

	// Update center of mass velocity.
	m_linearVelocity += Cross(m_angularVelocity, m_sweep.c - oldCenter);
}

void Body::SetMassData(const MassData* massData)
{
	assert(m_world->IsLocked() == false);
	if (m_world->IsLocked() == true)
	{
		return;
	}

	if (m_type != dynamicBody)
	{
		return;
	}

	m_invMass = 0.0f;
	m_I = 0.0f;
	m_invI = 0.0f;

	m_mass = massData->mass;
	if (m_mass <= 0.0f)
	{
		m_mass = 1.0f;
	}

	m_invMass = 1.0f / m_mass;

	if (massData->I > 0.0f && (m_flags & Body::e_fixedRotationFlag) == 0)
	{
		m_I = massData->I - m_mass * Dot(massData->center, massData->center);
		assert(m_I > 0.0f);
		m_invI = 1.0f / m_I;
	}

	// Move center of mass.
	Vec2 oldCenter = m_sweep.c;
	m_sweep.localCenter =  massData->center;
	m_sweep.c0 = m_sweep.c = Mul(m_xf, m_sweep.localCenter);

	// Update center of mass velocity.
	m_linearVelocity += Cross(m_angularVelocity, m_sweep.c - oldCenter);
}

bool Body::ShouldCollide(const Body* other) const
{
	// At least one body should be dynamic.
	if (m_type != dynamicBody && other->m_type != dynamicBody)
	{
		return false;
	}

	// Does a joint prevent collision?
	for (JointEdge* jn = m_jointList; jn; jn = jn->next)
	{
		if (jn->other == other)
		{
			if (jn->joint->m_collideConnected == false)
			{
				return false;
			}
		}
	}

	return true;
}

void Body::SetTransform(const Vec2& position, float32 angle)
{
	assert(m_world->IsLocked() == false);
	if (m_world->IsLocked() == true)
	{
		return;
	}

	m_xf.q.Set(angle);
	m_xf.p = position;

	m_sweep.c = Mul(m_xf, m_sweep.localCenter);
	m_sweep.a = angle;

	m_sweep.c0 = m_sweep.c;
	m_sweep.a0 = angle;

	BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
	for (Fixture* f = m_fixtureList; f; f = f->m_next)
	{
		f->Synchronize(broadPhase, m_xf, m_xf);
	}
}

void Body::SynchronizeFixtures()
{
	Transform xf1;
	xf1.q.Set(m_sweep.a0);
	xf1.p = m_sweep.c0 - Mul(xf1.q, m_sweep.localCenter);

	BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
	for (Fixture* f = m_fixtureList; f; f = f->m_next)
	{
		f->Synchronize(broadPhase, xf1, m_xf);
	}
}

void Body::SetActive(bool flag)
{
	assert(m_world->IsLocked() == false);

	if (flag == IsActive())
	{
		return;
	}

	if (flag)
	{
		m_flags |= e_activeFlag;

		// Create all proxies.
		BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
		for (Fixture* f = m_fixtureList; f; f = f->m_next)
		{
			f->CreateProxies(broadPhase, m_xf);
		}

		// Contacts are created the next time step.
	}
	else
	{
		m_flags &= ~e_activeFlag;

		// Destroy all proxies.
		BroadPhase* broadPhase = &m_world->m_contactManager.m_broadPhase;
		for (Fixture* f = m_fixtureList; f; f = f->m_next)
		{
			f->DestroyProxies(broadPhase);
		}

		// Destroy the attached contacts.
		ContactEdge* ce = m_contactList;
		while (ce)
		{
			ContactEdge* ce0 = ce;
			ce = ce->next;
			m_world->m_contactManager.Destroy(ce0->contact);
		}
		m_contactList = NULL;
	}
}

void Body::SetFixedRotation(bool flag)
{
	bool status = (m_flags & e_fixedRotationFlag) == e_fixedRotationFlag;
	if (status == flag)
	{
		return;
	}

	if (flag)
	{
		m_flags |= e_fixedRotationFlag;
	}
	else
	{
		m_flags &= ~e_fixedRotationFlag;
	}

	m_angularVelocity = 0.0f;

	ResetMassData();
}

void Body::Dump()
{
	int32 bodyIndex = m_islandIndex;

	Log("{\n");
	Log("  b2::BodyDef bd;\n");
	Log("  bd.type = b2::BodyType(%d);\n", m_type);
	Log("  bd.position.Set(%.15lef, %.15lef);\n", m_xf.p.x, m_xf.p.y);
	Log("  bd.angle = %.15lef;\n", m_sweep.a);
	Log("  bd.linearVelocity.Set(%.15lef, %.15lef);\n", m_linearVelocity.x, m_linearVelocity.y);
	Log("  bd.angularVelocity = %.15lef;\n", m_angularVelocity);
	Log("  bd.linearDamping = %.15lef;\n", m_linearDamping);
	Log("  bd.angularDamping = %.15lef;\n", m_angularDamping);
	Log("  bd.allowSleep = bool(%d);\n", m_flags & e_autoSleepFlag);
	Log("  bd.awake = bool(%d);\n", m_flags & e_awakeFlag);
	Log("  bd.fixedRotation = bool(%d);\n", m_flags & e_fixedRotationFlag);
	Log("  bd.bullet = bool(%d);\n", m_flags & e_bulletFlag);
	Log("  bd.active = bool(%d);\n", m_flags & e_activeFlag);
	Log("  bd.gravityScale = %.15lef;\n", m_gravityScale);
	Log("  bodies[%d] = m_world->CreateBody(&bd);\n", m_islandIndex);
	Log("\n");
	for (Fixture* f = m_fixtureList; f; f = f->m_next)
	{
		Log("  {\n");
		f->Dump(bodyIndex);
		Log("  }\n");
	}
	Log("}\n");
}

} // namespace b2
