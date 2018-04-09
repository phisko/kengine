/*
* Copyright (c) 2006-2011 Erin Catto http://www.box2d.org
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

#include <Box2D/Dynamics/World.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <Box2D/Dynamics/Island.hpp>
#include <Box2D/Dynamics/Joints/PulleyJoint.hpp>
#include <Box2D/Dynamics/Contacts/Contact.hpp>
#include <Box2D/Dynamics/Contacts/ContactSolver.hpp>
#include <Box2D/Collision/Collision.hpp>
#include <Box2D/Collision/BroadPhase.hpp>
#include <Box2D/Collision/Shapes/CircleShape.hpp>
#include <Box2D/Collision/Shapes/EdgeShape.hpp>
#include <Box2D/Collision/Shapes/ChainShape.hpp>
#include <Box2D/Collision/Shapes/PolygonShape.hpp>
#include <Box2D/Collision/TimeOfImpact.hpp>
#include <Box2D/Common/Draw.hpp>
#include <Box2D/Common/Timer.hpp>
#include <new>

namespace b2
{

World::World(const Vec2& gravity)
{
	m_destructionListener = NULL;
	g_debugDraw = NULL;

	m_bodyList = NULL;
	m_jointList = NULL;

	m_bodyCount = 0;
	m_jointCount = 0;

	m_warmStarting = true;
	m_continuousPhysics = true;
	m_subStepping = false;

	m_stepComplete = true;

	m_allowSleep = true;
	m_gravity = gravity;

	m_flags = e_clearForces;

	m_inv_dt0 = 0.0f;

	m_contactManager.m_allocator = &m_blockAllocator;

	memset(&m_profile, 0, sizeof(Profile));
}

World::~World()
{
	// Some shapes allocate using Alloc.
	Body* b = m_bodyList;
	while (b)
	{
		Body* bNext = b->m_next;

		Fixture* f = b->m_fixtureList;
		while (f)
		{
			Fixture* fNext = f->m_next;
			f->m_proxyCount = 0;
			f->Destroy(&m_blockAllocator);
			f = fNext;
		}

		b = bNext;
	}
}

void World::SetDestructionListener(DestructionListener* listener)
{
	m_destructionListener = listener;
}

void World::SetContactFilter(ContactFilter* filter)
{
	m_contactManager.m_contactFilter = filter;
}

void World::SetContactListener(ContactListener* listener)
{
	m_contactManager.m_contactListener = listener;
}

void World::SetDebugDraw(Draw* debugDraw)
{
	g_debugDraw = debugDraw;
}

Body* World::CreateBody(const BodyDef* def)
{
	assert(IsLocked() == false);
	if (IsLocked())
	{
		return NULL;
	}

	void* mem = m_blockAllocator.Allocate(sizeof(Body));
	Body* b = new (mem) Body(def, this);

	// Add to world doubly linked list.
	b->m_prev = NULL;
	b->m_next = m_bodyList;
	if (m_bodyList)
	{
		m_bodyList->m_prev = b;
	}
	m_bodyList = b;
	++m_bodyCount;

	return b;
}

void World::DestroyBody(Body* b)
{
	assert(m_bodyCount > 0);
	assert(IsLocked() == false);
	if (IsLocked())
	{
		return;
	}

	// Delete the attached joints.
	JointEdge* je = b->m_jointList;
	while (je)
	{
		JointEdge* je0 = je;
		je = je->next;

		if (m_destructionListener)
		{
			m_destructionListener->SayGoodbye(je0->joint);
		}

		DestroyJoint(je0->joint);

		b->m_jointList = je;
	}
	b->m_jointList = NULL;

	// Delete the attached contacts.
	ContactEdge* ce = b->m_contactList;
	while (ce)
	{
		ContactEdge* ce0 = ce;
		ce = ce->next;
		m_contactManager.Destroy(ce0->contact);
	}
	b->m_contactList = NULL;

	// Delete the attached fixtures. This destroys broad-phase proxies.
	Fixture* f = b->m_fixtureList;
	while (f)
	{
		Fixture* f0 = f;
		f = f->m_next;

		if (m_destructionListener)
		{
			m_destructionListener->SayGoodbye(f0);
		}

		f0->DestroyProxies(&m_contactManager.m_broadPhase);
		f0->Destroy(&m_blockAllocator);
		f0->~Fixture();
		m_blockAllocator.Free(f0, sizeof(Fixture));

		b->m_fixtureList = f;
		b->m_fixtureCount -= 1;
	}
	b->m_fixtureList = NULL;
	b->m_fixtureCount = 0;

	// Remove world body list.
	if (b->m_prev)
	{
		b->m_prev->m_next = b->m_next;
	}

	if (b->m_next)
	{
		b->m_next->m_prev = b->m_prev;
	}

	if (b == m_bodyList)
	{
		m_bodyList = b->m_next;
	}

	--m_bodyCount;
	b->~Body();
	m_blockAllocator.Free(b, sizeof(Body));
}

Joint* World::CreateJoint(const JointDef* def)
{
	assert(IsLocked() == false);
	if (IsLocked())
	{
		return NULL;
	}

	Joint* j = Joint::Create(def, &m_blockAllocator);

	// Connect to the world list.
	j->m_prev = NULL;
	j->m_next = m_jointList;
	if (m_jointList)
	{
		m_jointList->m_prev = j;
	}
	m_jointList = j;
	++m_jointCount;

	// Connect to the bodies' doubly linked lists.
	j->m_edgeA.joint = j;
	j->m_edgeA.other = j->m_bodyB;
	j->m_edgeA.prev = NULL;
	j->m_edgeA.next = j->m_bodyA->m_jointList;
	if (j->m_bodyA->m_jointList) j->m_bodyA->m_jointList->prev = &j->m_edgeA;
	j->m_bodyA->m_jointList = &j->m_edgeA;

	j->m_edgeB.joint = j;
	j->m_edgeB.other = j->m_bodyA;
	j->m_edgeB.prev = NULL;
	j->m_edgeB.next = j->m_bodyB->m_jointList;
	if (j->m_bodyB->m_jointList) j->m_bodyB->m_jointList->prev = &j->m_edgeB;
	j->m_bodyB->m_jointList = &j->m_edgeB;

	Body* bodyA = def->bodyA;
	Body* bodyB = def->bodyB;

	// If the joint prevents collisions, then flag any contacts for filtering.
	if (def->collideConnected == false)
	{
		ContactEdge* edge = bodyB->GetContactList();
		while (edge)
		{
			if (edge->other == bodyA)
			{
				// Flag the contact for filtering at the next time step (where either
				// body is awake).
				edge->contact->FlagForFiltering();
			}

			edge = edge->next;
		}
	}

	// Note: creating a joint doesn't wake the bodies.

	return j;
}

void World::DestroyJoint(Joint* j)
{
	assert(IsLocked() == false);
	if (IsLocked())
	{
		return;
	}

	bool collideConnected = j->m_collideConnected;

	// Remove from the doubly linked list.
	if (j->m_prev)
	{
		j->m_prev->m_next = j->m_next;
	}

	if (j->m_next)
	{
		j->m_next->m_prev = j->m_prev;
	}

	if (j == m_jointList)
	{
		m_jointList = j->m_next;
	}

	// Disconnect from island graph.
	Body* bodyA = j->m_bodyA;
	Body* bodyB = j->m_bodyB;

	// Wake up connected bodies.
	bodyA->SetAwake(true);
	bodyB->SetAwake(true);

	// Remove from body 1.
	if (j->m_edgeA.prev)
	{
		j->m_edgeA.prev->next = j->m_edgeA.next;
	}

	if (j->m_edgeA.next)
	{
		j->m_edgeA.next->prev = j->m_edgeA.prev;
	}

	if (&j->m_edgeA == bodyA->m_jointList)
	{
		bodyA->m_jointList = j->m_edgeA.next;
	}

	j->m_edgeA.prev = NULL;
	j->m_edgeA.next = NULL;

	// Remove from body 2
	if (j->m_edgeB.prev)
	{
		j->m_edgeB.prev->next = j->m_edgeB.next;
	}

	if (j->m_edgeB.next)
	{
		j->m_edgeB.next->prev = j->m_edgeB.prev;
	}

	if (&j->m_edgeB == bodyB->m_jointList)
	{
		bodyB->m_jointList = j->m_edgeB.next;
	}

	j->m_edgeB.prev = NULL;
	j->m_edgeB.next = NULL;

	Joint::Destroy(j, &m_blockAllocator);

	assert(m_jointCount > 0);
	--m_jointCount;

	// If the joint prevents collisions, then flag any contacts for filtering.
	if (collideConnected == false)
	{
		ContactEdge* edge = bodyB->GetContactList();
		while (edge)
		{
			if (edge->other == bodyA)
			{
				// Flag the contact for filtering at the next time step (where either
				// body is awake).
				edge->contact->FlagForFiltering();
			}

			edge = edge->next;
		}
	}
}

//
void World::SetAllowSleeping(bool flag)
{
	if (flag == m_allowSleep)
	{
		return;
	}

	m_allowSleep = flag;
	if (m_allowSleep == false)
	{
		for (Body* b = m_bodyList; b; b = b->m_next)
		{
			b->SetAwake(true);
		}
	}
}

// Find islands, integrate and solve constraints, solve position constraints
void World::Solve(const TimeStep& step)
{
	m_profile.solveInit = 0.0f;
	m_profile.solveVelocity = 0.0f;
	m_profile.solvePosition = 0.0f;

	// Size the island for the worst case.
	Island island(m_bodyCount,
				  m_contactManager.m_contactCount,
				  m_jointCount,
				  &m_stackAllocator,
				  m_contactManager.m_contactListener);

	// Clear all the island flags.
	for (Body* b = m_bodyList; b; b = b->m_next)
	{
		b->m_flags &= ~Body::e_islandFlag;
	}
	for (Contact* c = m_contactManager.m_contactList; c; c = c->m_next)
	{
		c->m_flags &= ~Contact::e_islandFlag;
	}
	for (Joint* j = m_jointList; j; j = j->m_next)
	{
		j->m_islandFlag = false;
	}

	// Build and simulate all awake islands.
	int32 stackSize = m_bodyCount;
	Body** stack = (Body**)m_stackAllocator.Allocate(stackSize * sizeof(Body*));
	for (Body* seed = m_bodyList; seed; seed = seed->m_next)
	{
		if (seed->m_flags & Body::e_islandFlag)
		{
			continue;
		}

		if (seed->IsAwake() == false || seed->IsActive() == false)
		{
			continue;
		}

		// The seed can be dynamic or kinematic.
		if (seed->GetType() == staticBody)
		{
			continue;
		}

		// Reset island and stack.
		island.Clear();
		int32 stackCount = 0;
		stack[stackCount++] = seed;
		seed->m_flags |= Body::e_islandFlag;

		// Perform a depth first search (DFS) on the constraint graph.
		while (stackCount > 0)
		{
			// Grab the next body off the stack and add it to the island.
			Body* b = stack[--stackCount];
			assert(b->IsActive() == true);
			island.Add(b);

			// Make sure the body is awake.
			b->SetAwake(true);

			// To keep islands as small as possible, we don't
			// propagate islands across static bodies.
			if (b->GetType() == staticBody)
			{
				continue;
			}

			// Search all contacts connected to this body.
			for (ContactEdge* ce = b->m_contactList; ce; ce = ce->next)
			{
				Contact* contact = ce->contact;

				// Has this contact already been added to an island?
				if (contact->m_flags & Contact::e_islandFlag)
				{
					continue;
				}

				// Is this contact solid and touching?
				if (contact->IsEnabled() == false ||
					contact->IsTouching() == false)
				{
					continue;
				}

				// Skip sensors.
				bool sensorA = contact->m_fixtureA->m_isSensor;
				bool sensorB = contact->m_fixtureB->m_isSensor;
				if (sensorA || sensorB)
				{
					continue;
				}

				island.Add(contact);
				contact->m_flags |= Contact::e_islandFlag;

				Body* other = ce->other;

				// Was the other body already added to this island?
				if (other->m_flags & Body::e_islandFlag)
				{
					continue;
				}

				assert(stackCount < stackSize);
				stack[stackCount++] = other;
				other->m_flags |= Body::e_islandFlag;
			}

			// Search all joints connect to this body.
			for (JointEdge* je = b->m_jointList; je; je = je->next)
			{
				if (je->joint->m_islandFlag == true)
				{
					continue;
				}

				Body* other = je->other;

				// Don't simulate joints connected to inactive bodies.
				if (other->IsActive() == false)
				{
					continue;
				}

				island.Add(je->joint);
				je->joint->m_islandFlag = true;

				if (other->m_flags & Body::e_islandFlag)
				{
					continue;
				}

				assert(stackCount < stackSize);
				stack[stackCount++] = other;
				other->m_flags |= Body::e_islandFlag;
			}
		}

		Profile profile;
		island.Solve(&profile, step, m_gravity, m_allowSleep);
		m_profile.solveInit += profile.solveInit;
		m_profile.solveVelocity += profile.solveVelocity;
		m_profile.solvePosition += profile.solvePosition;

		// Post solve cleanup.
		for (int32 i = 0; i < island.m_bodyCount; ++i)
		{
			// Allow static bodies to participate in other islands.
			Body* b = island.m_bodies[i];
			if (b->GetType() == staticBody)
			{
				b->m_flags &= ~Body::e_islandFlag;
			}
		}
	}

	m_stackAllocator.Free(stack);

	{
		Timer timer;
		// Synchronize fixtures, check for out of range bodies.
		for (Body* b = m_bodyList; b; b = b->GetNext())
		{
			// If a body was not in an island then it did not move.
			if ((b->m_flags & Body::e_islandFlag) == 0)
			{
				continue;
			}

			if (b->GetType() == staticBody)
			{
				continue;
			}

			// Update fixtures (for broad-phase).
			b->SynchronizeFixtures();
		}

		// Look for new contacts.
		m_contactManager.FindNewContacts();
		m_profile.broadphase = timer.GetMilliseconds();
	}
}

// Find TOI contacts and solve them.
void World::SolveTOI(const TimeStep& step)
{
	Island island(2 * maxTOIContacts, maxTOIContacts, 0, &m_stackAllocator, m_contactManager.m_contactListener);

	if (m_stepComplete)
	{
		for (Body* b = m_bodyList; b; b = b->m_next)
		{
			b->m_flags &= ~Body::e_islandFlag;
			b->m_sweep.alpha0 = 0.0f;
		}

		for (Contact* c = m_contactManager.m_contactList; c; c = c->m_next)
		{
			// Invalidate TOI
			c->m_flags &= ~(Contact::e_toiFlag | Contact::e_islandFlag);
			c->m_toiCount = 0;
			c->m_toi = 1.0f;
		}
	}

	// Find TOI events and solve them.
	for (;;)
	{
		// Find the first TOI.
		Contact* minContact = NULL;
		float32 minAlpha = 1.0f;

		for (Contact* c = m_contactManager.m_contactList; c; c = c->m_next)
		{
			// Is this contact disabled?
			if (c->IsEnabled() == false)
			{
				continue;
			}

			// Prevent excessive sub-stepping.
			if (c->m_toiCount > maxSubSteps)
			{
				continue;
			}

			float32 alpha = 1.0f;
			if (c->m_flags & Contact::e_toiFlag)
			{
				// This contact has a valid cached TOI.
				alpha = c->m_toi;
			}
			else
			{
				Fixture* fA = c->GetFixtureA();
				Fixture* fB = c->GetFixtureB();

				// Is there a sensor?
				if (fA->IsSensor() || fB->IsSensor())
				{
					continue;
				}

				Body* bA = fA->GetBody();
				Body* bB = fB->GetBody();

				BodyType typeA = bA->m_type;
				BodyType typeB = bB->m_type;
				assert(typeA == dynamicBody || typeB == dynamicBody);

				bool activeA = bA->IsAwake() && typeA != staticBody;
				bool activeB = bB->IsAwake() && typeB != staticBody;

				// Is at least one body active (awake and dynamic or kinematic)?
				if (activeA == false && activeB == false)
				{
					continue;
				}

				bool collideA = bA->IsBullet() || typeA != dynamicBody;
				bool collideB = bB->IsBullet() || typeB != dynamicBody;

				// Are these two non-bullet dynamic bodies?
				if (collideA == false && collideB == false)
				{
					continue;
				}

				// Compute the TOI for this contact.
				// Put the sweeps onto the same time interval.
				float32 alpha0 = bA->m_sweep.alpha0;

				if (bA->m_sweep.alpha0 < bB->m_sweep.alpha0)
				{
					alpha0 = bB->m_sweep.alpha0;
					bA->m_sweep.Advance(alpha0);
				}
				else if (bB->m_sweep.alpha0 < bA->m_sweep.alpha0)
				{
					alpha0 = bA->m_sweep.alpha0;
					bB->m_sweep.Advance(alpha0);
				}

				assert(alpha0 < 1.0f);

				int32 indexA = c->GetChildIndexA();
				int32 indexB = c->GetChildIndexB();

				// Compute the time of impact in interval [0, minTOI]
				TOIInput input;
				input.proxyA.Set(fA->GetShape(), indexA);
				input.proxyB.Set(fB->GetShape(), indexB);
				input.sweepA = bA->m_sweep;
				input.sweepB = bB->m_sweep;
				input.tMax = 1.0f;

				TOIOutput output;
				TimeOfImpact(&output, &input);

				// Beta is the fraction of the remaining portion of the .
				float32 beta = output.t;
				if (output.state == TOIOutput::e_touching)
				{
					alpha = Min(alpha0 + (1.0f - alpha0) * beta, 1.0f);
				}
				else
				{
					alpha = 1.0f;
				}

				c->m_toi = alpha;
				c->m_flags |= Contact::e_toiFlag;
			}

			if (alpha < minAlpha)
			{
				// This is the minimum TOI found so far.
				minContact = c;
				minAlpha = alpha;
			}
		}

		if (minContact == NULL || 1.0f - 10.0f * epsilon < minAlpha)
		{
			// No more TOI events. Done!
			m_stepComplete = true;
			break;
		}

		// Advance the bodies to the TOI.
		Fixture* fA = minContact->GetFixtureA();
		Fixture* fB = minContact->GetFixtureB();
		Body* bA = fA->GetBody();
		Body* bB = fB->GetBody();

		Sweep backup1 = bA->m_sweep;
		Sweep backup2 = bB->m_sweep;

		bA->Advance(minAlpha);
		bB->Advance(minAlpha);

		// The TOI contact likely has some new contact points.
		minContact->Update(m_contactManager.m_contactListener);
		minContact->m_flags &= ~Contact::e_toiFlag;
		++minContact->m_toiCount;

		// Is the contact solid?
		if (minContact->IsEnabled() == false || minContact->IsTouching() == false)
		{
			// Restore the sweeps.
			minContact->SetEnabled(false);
			bA->m_sweep = backup1;
			bB->m_sweep = backup2;
			bA->SynchronizeTransform();
			bB->SynchronizeTransform();
			continue;
		}

		bA->SetAwake(true);
		bB->SetAwake(true);

		// Build the island
		island.Clear();
		island.Add(bA);
		island.Add(bB);
		island.Add(minContact);

		bA->m_flags |= Body::e_islandFlag;
		bB->m_flags |= Body::e_islandFlag;
		minContact->m_flags |= Contact::e_islandFlag;

		// Get contacts on bodyA and bodyB.
		Body* bodies[2] = {bA, bB};
		for (int32 i = 0; i < 2; ++i)
		{
			Body* body = bodies[i];
			if (body->m_type == dynamicBody)
			{
				for (ContactEdge* ce = body->m_contactList; ce; ce = ce->next)
				{
					if (island.m_bodyCount == island.m_bodyCapacity)
					{
						break;
					}

					if (island.m_contactCount == island.m_contactCapacity)
					{
						break;
					}

					Contact* contact = ce->contact;

					// Has this contact already been added to the island?
					if (contact->m_flags & Contact::e_islandFlag)
					{
						continue;
					}

					// Only add static, kinematic, or bullet bodies.
					Body* other = ce->other;
					if (other->m_type == dynamicBody &&
						body->IsBullet() == false && other->IsBullet() == false)
					{
						continue;
					}

					// Skip sensors.
					bool sensorA = contact->m_fixtureA->m_isSensor;
					bool sensorB = contact->m_fixtureB->m_isSensor;
					if (sensorA || sensorB)
					{
						continue;
					}

					// Tentatively advance the body to the TOI.
					Sweep backup = other->m_sweep;
					if ((other->m_flags & Body::e_islandFlag) == 0)
					{
						other->Advance(minAlpha);
					}

					// Update the contact points
					contact->Update(m_contactManager.m_contactListener);

					// Was the contact disabled by the user?
					if (contact->IsEnabled() == false)
					{
						other->m_sweep = backup;
						other->SynchronizeTransform();
						continue;
					}

					// Are there contact points?
					if (contact->IsTouching() == false)
					{
						other->m_sweep = backup;
						other->SynchronizeTransform();
						continue;
					}

					// Add the contact to the island
					contact->m_flags |= Contact::e_islandFlag;
					island.Add(contact);

					// Has the other body already been added to the island?
					if (other->m_flags & Body::e_islandFlag)
					{
						continue;
					}
					
					// Add the other body to the island.
					other->m_flags |= Body::e_islandFlag;

					if (other->m_type != staticBody)
					{
						other->SetAwake(true);
					}

					island.Add(other);
				}
			}
		}

		TimeStep subStep;
		subStep.dt = (1.0f - minAlpha) * step.dt;
		subStep.inv_dt = 1.0f / subStep.dt;
		subStep.dtRatio = 1.0f;
		subStep.positionIterations = 20;
		subStep.velocityIterations = step.velocityIterations;
		subStep.warmStarting = false;
		island.SolveTOI(subStep, bA->m_islandIndex, bB->m_islandIndex);

		// Reset island flags and synchronize broad-phase proxies.
		for (int32 i = 0; i < island.m_bodyCount; ++i)
		{
			Body* body = island.m_bodies[i];
			body->m_flags &= ~Body::e_islandFlag;

			if (body->m_type != dynamicBody)
			{
				continue;
			}

			body->SynchronizeFixtures();

			// Invalidate all contact TOIs on this displaced body.
			for (ContactEdge* ce = body->m_contactList; ce; ce = ce->next)
			{
				ce->contact->m_flags &= ~(Contact::e_toiFlag | Contact::e_islandFlag);
			}
		}

		// Commit fixture proxy movements to the broad-phase so that new contacts are created.
		// Also, some contacts can be destroyed.
		m_contactManager.FindNewContacts();

		if (m_subStepping)
		{
			m_stepComplete = false;
			break;
		}
	}
}

void World::Step(float32 dt, int32 velocityIterations, int32 positionIterations)
{
	Timer stepTimer;

	// If new fixtures were added, we need to find the new contacts.
	if (m_flags & e_newFixture)
	{
		m_contactManager.FindNewContacts();
		m_flags &= ~e_newFixture;
	}

	m_flags |= e_locked;

	TimeStep step;
	step.dt = dt;
	step.velocityIterations	= velocityIterations;
	step.positionIterations = positionIterations;
	if (dt > 0.0f)
	{
		step.inv_dt = 1.0f / dt;
	}
	else
	{
		step.inv_dt = 0.0f;
	}

	step.dtRatio = m_inv_dt0 * dt;

	step.warmStarting = m_warmStarting;
	
	// Update contacts. This is where some contacts are destroyed.
	{
		Timer timer;
		m_contactManager.Collide();
		m_profile.collide = timer.GetMilliseconds();
	}

	// Integrate velocities, solve velocity constraints, and integrate positions.
	if (m_stepComplete && step.dt > 0.0f)
	{
		Timer timer;
		Solve(step);
		m_profile.solve = timer.GetMilliseconds();
	}

	// Handle TOI events.
	if (m_continuousPhysics && step.dt > 0.0f)
	{
		Timer timer;
		SolveTOI(step);
		m_profile.solveTOI = timer.GetMilliseconds();
	}

	if (step.dt > 0.0f)
	{
		m_inv_dt0 = step.inv_dt;
	}

	if (m_flags & e_clearForces)
	{
		ClearForces();
	}

	m_flags &= ~e_locked;

	m_profile.step = stepTimer.GetMilliseconds();
}

void World::ClearForces()
{
	for (Body* body = m_bodyList; body; body = body->GetNext())
	{
		body->m_force.SetZero();
		body->m_torque = 0.0f;
	}
}

struct WorldQueryWrapper
{
	bool QueryCallback(int32 proxyId)
	{
		FixtureProxy* proxy = (FixtureProxy*)broadPhase->GetUserData(proxyId);
		return callback->ReportFixture(proxy->fixture);
	}

	const BroadPhase* broadPhase;
	b2::QueryCallback* callback;
};

void World::QueryAABB(QueryCallback* callback, const AABB& aabb) const
{
	WorldQueryWrapper wrapper;
	wrapper.broadPhase = &m_contactManager.m_broadPhase;
	wrapper.callback = callback;
	m_contactManager.m_broadPhase.Query(&wrapper, aabb);
}

struct WorldRayCastWrapper
{
	float32 RayCastCallback(const RayCastInput& input, int32 proxyId)
	{
		void* userData = broadPhase->GetUserData(proxyId);
		FixtureProxy* proxy = (FixtureProxy*)userData;
		Fixture* fixture = proxy->fixture;
		int32 index = proxy->childIndex;
		RayCastOutput output;
		bool hit = fixture->RayCast(&output, input, index);

		if (hit)
		{
			float32 fraction = output.fraction;
			Vec2 point = (1.0f - fraction) * input.p1 + fraction * input.p2;
			return callback->ReportFixture(fixture, point, output.normal, fraction);
		}

		return input.maxFraction;
	}

	const BroadPhase* broadPhase;
	b2::RayCastCallback* callback;
};

void World::RayCast(RayCastCallback* callback, const Vec2& point1, const Vec2& point2) const
{
	WorldRayCastWrapper wrapper;
	wrapper.broadPhase = &m_contactManager.m_broadPhase;
	wrapper.callback = callback;
	RayCastInput input;
	input.maxFraction = 1.0f;
	input.p1 = point1;
	input.p2 = point2;
	m_contactManager.m_broadPhase.RayCast(&wrapper, input);
}

void World::DrawShape(Fixture* fixture, const Transform& xf, const Color& color)
{
	switch (fixture->GetType())
	{
	case Shape::e_circle:
		{
			CircleShape* circle = (CircleShape*)fixture->GetShape();

			Vec2 center = Mul(xf, circle->m_p);
			float32 radius = circle->m_radius;
			Vec2 axis = Mul(xf.q, Vec2(1.0f, 0.0f));

			g_debugDraw->DrawSolidCircle(center, radius, axis, color);
		}
		break;

	case Shape::e_edge:
		{
			EdgeShape* edge = (EdgeShape*)fixture->GetShape();
			Vec2 v1 = Mul(xf, edge->m_vertex1);
			Vec2 v2 = Mul(xf, edge->m_vertex2);
			g_debugDraw->DrawSegment(v1, v2, color);
		}
		break;

	case Shape::e_chain:
		{
			ChainShape* chain = (ChainShape*)fixture->GetShape();
			int32 count = chain->m_count;
			const Vec2* vertices = chain->m_vertices;

			Vec2 v1 = Mul(xf, vertices[0]);
			for (int32 i = 1; i < count; ++i)
			{
				Vec2 v2 = Mul(xf, vertices[i]);
				g_debugDraw->DrawSegment(v1, v2, color);
				g_debugDraw->DrawCircle(v1, 0.05f, color);
				v1 = v2;
			}
		}
		break;

	case Shape::e_polygon:
		{
			PolygonShape* poly = (PolygonShape*)fixture->GetShape();
			int32 vertexCount = poly->m_count;
			assert(vertexCount <= maxPolygonVertices);
			Vec2 vertices[maxPolygonVertices];

			for (int32 i = 0; i < vertexCount; ++i)
			{
				vertices[i] = Mul(xf, poly->m_vertices[i]);
			}

			g_debugDraw->DrawSolidPolygon(vertices, vertexCount, color);
		}
		break;
            
    default:
        break;
	}
}

void World::DrawJoint(Joint* joint)
{
	Body* bodyA = joint->GetBodyA();
	Body* bodyB = joint->GetBodyB();
	const Transform& xf1 = bodyA->GetTransform();
	const Transform& xf2 = bodyB->GetTransform();
	Vec2 x1 = xf1.p;
	Vec2 x2 = xf2.p;
	Vec2 p1 = joint->GetAnchorA();
	Vec2 p2 = joint->GetAnchorB();

	Color color(0.5f, 0.8f, 0.8f);

	switch (joint->GetType())
	{
	case e_distanceJoint:
		g_debugDraw->DrawSegment(p1, p2, color);
		break;

	case e_pulleyJoint:
		{
			PulleyJoint* pulley = (PulleyJoint*)joint;
			Vec2 s1 = pulley->GetGroundAnchorA();
			Vec2 s2 = pulley->GetGroundAnchorB();
			g_debugDraw->DrawSegment(s1, p1, color);
			g_debugDraw->DrawSegment(s2, p2, color);
			g_debugDraw->DrawSegment(s1, s2, color);
		}
		break;

	case e_mouseJoint:
		// don't draw this
		break;

	default:
		g_debugDraw->DrawSegment(x1, p1, color);
		g_debugDraw->DrawSegment(p1, p2, color);
		g_debugDraw->DrawSegment(x2, p2, color);
	}
}

void World::DrawDebugData()
{
	if (g_debugDraw == NULL)
	{
		return;
	}

	uint32 flags = g_debugDraw->GetFlags();

	if (flags & Draw::e_shapeBit)
	{
		for (Body* b = m_bodyList; b; b = b->GetNext())
		{
			const Transform& xf = b->GetTransform();
			for (Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
			{
				if (b->IsActive() == false)
				{
					DrawShape(f, xf, Color(0.5f, 0.5f, 0.3f));
				}
				else if (b->GetType() == staticBody)
				{
					DrawShape(f, xf, Color(0.5f, 0.9f, 0.5f));
				}
				else if (b->GetType() == kinematicBody)
				{
					DrawShape(f, xf, Color(0.5f, 0.5f, 0.9f));
				}
				else if (b->IsAwake() == false)
				{
					DrawShape(f, xf, Color(0.6f, 0.6f, 0.6f));
				}
				else
				{
					DrawShape(f, xf, Color(0.9f, 0.7f, 0.7f));
				}
			}
		}
	}

	if (flags & Draw::e_jointBit)
	{
		for (Joint* j = m_jointList; j; j = j->GetNext())
		{
			DrawJoint(j);
		}
	}

	if (flags & Draw::e_pairBit)
	{
		Color color(0.3f, 0.9f, 0.9f);
		for (Contact* c = m_contactManager.m_contactList; c; c = c->GetNext())
		{
			//Fixture* fixtureA = c->GetFixtureA();
			//Fixture* fixtureB = c->GetFixtureB();

			//Vec2 cA = fixtureA->GetAABB().GetCenter();
			//Vec2 cB = fixtureB->GetAABB().GetCenter();

			//g_debugDraw->DrawSegment(cA, cB, color);
		}
	}

	if (flags & Draw::e_aabbBit)
	{
		Color color(0.9f, 0.3f, 0.9f);
		BroadPhase* bp = &m_contactManager.m_broadPhase;

		for (Body* b = m_bodyList; b; b = b->GetNext())
		{
			if (b->IsActive() == false)
			{
				continue;
			}

			for (Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
			{
				for (int32 i = 0; i < f->m_proxyCount; ++i)
				{
					FixtureProxy* proxy = f->m_proxies + i;
					AABB aabb = bp->GetFatAABB(proxy->proxyId);
					Vec2 vs[4];
					vs[0].Set(aabb.lowerBound.x, aabb.lowerBound.y);
					vs[1].Set(aabb.upperBound.x, aabb.lowerBound.y);
					vs[2].Set(aabb.upperBound.x, aabb.upperBound.y);
					vs[3].Set(aabb.lowerBound.x, aabb.upperBound.y);

					g_debugDraw->DrawPolygon(vs, 4, color);
				}
			}
		}
	}

	if (flags & Draw::e_centerOfMassBit)
	{
		for (Body* b = m_bodyList; b; b = b->GetNext())
		{
			Transform xf = b->GetTransform();
			xf.p = b->GetWorldCenter();
			g_debugDraw->DrawTransform(xf);
		}
	}
}

int32 World::GetProxyCount() const
{
	return m_contactManager.m_broadPhase.GetProxyCount();
}

int32 World::GetTreeHeight() const
{
	return m_contactManager.m_broadPhase.GetTreeHeight();
}

int32 World::GetTreeBalance() const
{
	return m_contactManager.m_broadPhase.GetTreeBalance();
}

float32 World::GetTreeQuality() const
{
	return m_contactManager.m_broadPhase.GetTreeQuality();
}

void World::ShiftOrigin(const Vec2& newOrigin)
{
	assert((m_flags & e_locked) == 0);
	if ((m_flags & e_locked) == e_locked)
	{
		return;
	}

	for (Body* b = m_bodyList; b; b = b->m_next)
	{
		b->m_xf.p -= newOrigin;
		b->m_sweep.c0 -= newOrigin;
		b->m_sweep.c -= newOrigin;
	}

	for (Joint* j = m_jointList; j; j = j->m_next)
	{
		j->ShiftOrigin(newOrigin);
	}

	m_contactManager.m_broadPhase.ShiftOrigin(newOrigin);
}

void World::Dump()
{
	if ((m_flags & e_locked) == e_locked)
	{
		return;
	}

	Log("b2::Vec2 g(%.15lef, %.15lef);\n", m_gravity.x, m_gravity.y);
	Log("m_world->SetGravity(g);\n");

	Log("b2::Body** bodies = (b2::Body**)b2Alloc(%d * sizeof(b2::Body*));\n", m_bodyCount);
	Log("b2::Joint** joints = (b2::Joint**)b2Alloc(%d * sizeof(b2::Joint*));\n", m_jointCount);
	int32 i = 0;
	for (Body* b = m_bodyList; b; b = b->m_next)
	{
		b->m_islandIndex = i;
		b->Dump();
		++i;
	}

	i = 0;
	for (Joint* j = m_jointList; j; j = j->m_next)
	{
		j->m_index = i;
		++i;
	}

	// First pass on joints, skip gear joints.
	for (Joint* j = m_jointList; j; j = j->m_next)
	{
		if (j->m_type == e_gearJoint)
		{
			continue;
		}

		Log("{\n");
		j->Dump();
		Log("}\n");
	}

	// Second pass on joints, only gear joints.
	for (Joint* j = m_jointList; j; j = j->m_next)
	{
		if (j->m_type != e_gearJoint)
		{
			continue;
		}

		Log("{\n");
		j->Dump();
		Log("}\n");
	}

	Log("b2::Free(joints);\n");
	Log("b2::Free(bodies);\n");
	Log("joints = NULL;\n");
	Log("bodies = NULL;\n");
}

} // namespace b2
