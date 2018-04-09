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

#include <Box2D/Dynamics/ContactManager.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <Box2D/Dynamics/WorldCallbacks.hpp>
#include <Box2D/Dynamics/Contacts/Contact.hpp>

namespace b2
{

ContactFilter defaultFilter;
ContactListener defaultListener;

ContactManager::ContactManager()
{
	m_contactList = NULL;
	m_contactCount = 0;
	m_contactFilter = &defaultFilter;
	m_contactListener = &defaultListener;
	m_allocator = NULL;
}

void ContactManager::Destroy(Contact* c)
{
	Fixture* fixtureA = c->GetFixtureA();
	Fixture* fixtureB = c->GetFixtureB();
	Body* bodyA = fixtureA->GetBody();
	Body* bodyB = fixtureB->GetBody();

	if (m_contactListener && c->IsTouching())
	{
		m_contactListener->EndContact(c);
	}

	// Remove from the world.
	if (c->m_prev)
	{
		c->m_prev->m_next = c->m_next;
	}

	if (c->m_next)
	{
		c->m_next->m_prev = c->m_prev;
	}

	if (c == m_contactList)
	{
		m_contactList = c->m_next;
	}

	// Remove from body 1
	if (c->m_nodeA.prev)
	{
		c->m_nodeA.prev->next = c->m_nodeA.next;
	}

	if (c->m_nodeA.next)
	{
		c->m_nodeA.next->prev = c->m_nodeA.prev;
	}

	if (&c->m_nodeA == bodyA->m_contactList)
	{
		bodyA->m_contactList = c->m_nodeA.next;
	}

	// Remove from body 2
	if (c->m_nodeB.prev)
	{
		c->m_nodeB.prev->next = c->m_nodeB.next;
	}

	if (c->m_nodeB.next)
	{
		c->m_nodeB.next->prev = c->m_nodeB.prev;
	}

	if (&c->m_nodeB == bodyB->m_contactList)
	{
		bodyB->m_contactList = c->m_nodeB.next;
	}

	// Call the factory.
	Contact::Destroy(c, m_allocator);
	--m_contactCount;
}

// This is the top level collision call for the time step. Here
// all the narrow phase collision is processed for the world
// contact list.
void ContactManager::Collide()
{
	// Update awake contacts.
	Contact* c = m_contactList;
	while (c)
	{
		Fixture* fixtureA = c->GetFixtureA();
		Fixture* fixtureB = c->GetFixtureB();
		int32 indexA = c->GetChildIndexA();
		int32 indexB = c->GetChildIndexB();
		Body* bodyA = fixtureA->GetBody();
		Body* bodyB = fixtureB->GetBody();
		 
		// Is this contact flagged for filtering?
		if (c->m_flags & Contact::e_filterFlag)
		{
			// Should these bodies collide?
			if (bodyB->ShouldCollide(bodyA) == false)
			{
				Contact* cNuke = c;
				c = cNuke->GetNext();
				Destroy(cNuke);
				continue;
			}

			// Check user filtering.
			if (m_contactFilter && m_contactFilter->ShouldCollide(fixtureA, fixtureB) == false)
			{
				Contact* cNuke = c;
				c = cNuke->GetNext();
				Destroy(cNuke);
				continue;
			}

			// Clear the filtering flag.
			c->m_flags &= ~Contact::e_filterFlag;
		}

		bool activeA = bodyA->IsAwake() && bodyA->m_type != staticBody;
		bool activeB = bodyB->IsAwake() && bodyB->m_type != staticBody;

		// At least one body must be awake and it must be dynamic or kinematic.
		if (activeA == false && activeB == false)
		{
			c = c->GetNext();
			continue;
		}

		int32 proxyIdA = fixtureA->m_proxies[indexA].proxyId;
		int32 proxyIdB = fixtureB->m_proxies[indexB].proxyId;
		bool overlap = m_broadPhase.TestOverlap(proxyIdA, proxyIdB);

		// Here we destroy contacts that cease to overlap in the broad-phase.
		if (overlap == false)
		{
			Contact* cNuke = c;
			c = cNuke->GetNext();
			Destroy(cNuke);
			continue;
		}

		// The contact persists.
		c->Update(m_contactListener);
		c = c->GetNext();
	}
}

void ContactManager::FindNewContacts()
{
	m_broadPhase.UpdatePairs(this);
}

void ContactManager::AddPair(void* proxyUserDataA, void* proxyUserDataB)
{
	FixtureProxy* proxyA = (FixtureProxy*)proxyUserDataA;
	FixtureProxy* proxyB = (FixtureProxy*)proxyUserDataB;

	Fixture* fixtureA = proxyA->fixture;
	Fixture* fixtureB = proxyB->fixture;

	int32 indexA = proxyA->childIndex;
	int32 indexB = proxyB->childIndex;

	Body* bodyA = fixtureA->GetBody();
	Body* bodyB = fixtureB->GetBody();

	// Are the fixtures on the same body?
	if (bodyA == bodyB)
	{
		return;
	}

	// TODO_ERIN use a hash table to remove a potential bottleneck when both
	// bodies have a lot of contacts.
	// Does a contact already exist?
	ContactEdge* edge = bodyB->GetContactList();
	while (edge)
	{
		if (edge->other == bodyA)
		{
			Fixture* fA = edge->contact->GetFixtureA();
			Fixture* fB = edge->contact->GetFixtureB();
			int32 iA = edge->contact->GetChildIndexA();
			int32 iB = edge->contact->GetChildIndexB();

			if (fA == fixtureA && fB == fixtureB && iA == indexA && iB == indexB)
			{
				// A contact already exists.
				return;
			}

			if (fA == fixtureB && fB == fixtureA && iA == indexB && iB == indexA)
			{
				// A contact already exists.
				return;
			}
		}

		edge = edge->next;
	}

	// Does a joint override collision? Is at least one body dynamic?
	if (bodyB->ShouldCollide(bodyA) == false)
	{
		return;
	}

	// Check user filtering.
	if (m_contactFilter && m_contactFilter->ShouldCollide(fixtureA, fixtureB) == false)
	{
		return;
	}

	// Call the factory.
	Contact* c = Contact::Create(fixtureA, indexA, fixtureB, indexB, m_allocator);
	if (c == NULL)
	{
		return;
	}

	// Contact creation may swap fixtures.
	fixtureA = c->GetFixtureA();
	fixtureB = c->GetFixtureB();
	indexA = c->GetChildIndexA();
	indexB = c->GetChildIndexB();
	bodyA = fixtureA->GetBody();
	bodyB = fixtureB->GetBody();

	// Insert into the world.
	c->m_prev = NULL;
	c->m_next = m_contactList;
	if (m_contactList != NULL)
	{
		m_contactList->m_prev = c;
	}
	m_contactList = c;

	// Connect to island graph.

	// Connect to body A
	c->m_nodeA.contact = c;
	c->m_nodeA.other = bodyB;

	c->m_nodeA.prev = NULL;
	c->m_nodeA.next = bodyA->m_contactList;
	if (bodyA->m_contactList != NULL)
	{
		bodyA->m_contactList->prev = &c->m_nodeA;
	}
	bodyA->m_contactList = &c->m_nodeA;

	// Connect to body B
	c->m_nodeB.contact = c;
	c->m_nodeB.other = bodyA;

	c->m_nodeB.prev = NULL;
	c->m_nodeB.next = bodyB->m_contactList;
	if (bodyB->m_contactList != NULL)
	{
		bodyB->m_contactList->prev = &c->m_nodeB;
	}
	bodyB->m_contactList = &c->m_nodeB;

	// Wake up the bodies
	if (fixtureA->IsSensor() == false && fixtureB->IsSensor() == false)
	{
		bodyA->SetAwake(true);
		bodyB->SetAwake(true);
	}

	++m_contactCount;
}

} // namespace b2
