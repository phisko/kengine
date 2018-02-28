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

#ifndef B2_CONTACT_HPP
#define B2_CONTACT_HPP

#include <Box2D/Common/Math.hpp>
#include <Box2D/Collision/Collision.hpp>
#include <Box2D/Collision/Shapes/Shape.hpp>
#include <Box2D/Dynamics/Fixture.hpp>

namespace b2
{

class Body;
class Contact;
class Fixture;
class World;
class BlockAllocator;
class StackAllocator;
class ContactListener;

/// Friction mixing law. The idea is to allow either fixture to drive the restitution to zero.
/// For example, anything slides on ice.
inline float32 MixFriction(float32 friction1, float32 friction2)
{
	return sqrtf(friction1 * friction2);
}

/// Restitution mixing law. The idea is allow for anything to bounce off an inelastic surface.
/// For example, a superball bounces on anything.
inline float32 MixRestitution(float32 restitution1, float32 restitution2)
{
	return restitution1 > restitution2 ? restitution1 : restitution2;
}

typedef Contact* ContactCreateFcn(	Fixture* fixtureA, int32 indexA,
									Fixture* fixtureB, int32 indexB,
									BlockAllocator* allocator);
typedef void ContactDestroyFcn(Contact* contact, BlockAllocator* allocator);

struct ContactRegister
{
	ContactCreateFcn* createFcn;
	ContactDestroyFcn* destroyFcn;
	bool primary;
};

/// A contact edge is used to connect bodies and contacts together
/// in a contact graph where each body is a node and each contact
/// is an edge. A contact edge belongs to a doubly linked list
/// maintained in each attached body. Each contact has two contact
/// nodes, one for each attached body.
struct ContactEdge
{
	Body* other;			///< provides quick access to the other body attached.
	Contact* contact;		///< the contact
	ContactEdge* prev;	///< the previous contact edge in the body's contact list
	ContactEdge* next;	///< the next contact edge in the body's contact list
};

/// The class manages contact between two shapes. A contact exists for each overlapping
/// AABB in the broad-phase (except if filtered). Therefore a contact object may exist
/// that has no contact points.
class Contact
{
public:

	/// Get the contact manifold. Do not modify the manifold unless you understand the
	/// internals of Box2D.
	Manifold* GetManifold();
	const Manifold* GetManifold() const;

	/// Get the world manifold.
	void GetWorldManifold(WorldManifold* worldManifold) const;

	/// Is this contact touching?
	bool IsTouching() const;

	/// Enable/disable this contact. This can be used inside the pre-solve
	/// contact listener. The contact is only disabled for the current
	/// time step (or sub-step in continuous collisions).
	void SetEnabled(bool flag);

	/// Has this contact been disabled?
	bool IsEnabled() const;

	/// Get the next contact in the world's contact list.
	Contact* GetNext();
	const Contact* GetNext() const;

	/// Get fixture A in this contact.
	Fixture* GetFixtureA();
	const Fixture* GetFixtureA() const;

	/// Get the child primitive index for fixture A.
	int32 GetChildIndexA() const;

	/// Get fixture B in this contact.
	Fixture* GetFixtureB();
	const Fixture* GetFixtureB() const;

	/// Get the child primitive index for fixture B.
	int32 GetChildIndexB() const;

	/// Override the default friction mixture. You can call this in b2::ContactListener::PreSolve.
	/// This value persists until set or reset.
	void SetFriction(float32 friction);

	/// Get the friction.
	float32 GetFriction() const;

	/// Reset the friction mixture to the default value.
	void ResetFriction();

	/// Override the default restitution mixture. You can call this in b2::ContactListener::PreSolve.
	/// The value persists until you set or reset.
	void SetRestitution(float32 restitution);

	/// Get the restitution.
	float32 GetRestitution() const;

	/// Reset the restitution to the default value.
	void ResetRestitution();

	/// Set the desired tangent speed for a conveyor belt behavior. In meters per second.
	void SetTangentSpeed(float32 speed);

	/// Get the desired tangent speed. In meters per second.
	float32 GetTangentSpeed() const;

	/// Evaluate this contact with your own manifold and transforms.
	virtual void Evaluate(Manifold* manifold, const Transform& xfA, const Transform& xfB) = 0;

protected:
	friend class ContactManager;
	friend class World;
	friend class ContactSolver;
	friend class Body;
	friend class Fixture;

	// Flags stored in m_flags
	enum
	{
		// Used when crawling contact graph when forming islands.
		e_islandFlag		= 0x0001,

        // Set when the shapes are touching.
		e_touchingFlag		= 0x0002,

		// This contact can be disabled (by user)
		e_enabledFlag		= 0x0004,

		// This contact needs filtering because a fixture filter was changed.
		e_filterFlag		= 0x0008,

		// This bullet contact had a TOI event
		e_bulletHitFlag		= 0x0010,

		// This contact has a valid TOI in m_toi
		e_toiFlag			= 0x0020
	};

	/// Flag this contact for filtering. Filtering will occur the next time step.
	void FlagForFiltering();

	static void AddType(ContactCreateFcn* createFcn, ContactDestroyFcn* destroyFcn,
						Shape::Type typeA, Shape::Type typeB);
	static void InitializeRegisters();
	static Contact* Create(Fixture* fixtureA, int32 indexA, Fixture* fixtureB, int32 indexB, BlockAllocator* allocator);
	static void Destroy(Contact* contact, Shape::Type typeA, Shape::Type typeB, BlockAllocator* allocator);
	static void Destroy(Contact* contact, BlockAllocator* allocator);

	Contact() : m_fixtureA(NULL), m_fixtureB(NULL) {}
	Contact(Fixture* fixtureA, int32 indexA, Fixture* fixtureB, int32 indexB);
	virtual ~Contact() {}

	void Update(ContactListener* listener);

	static ContactRegister s_registers[Shape::e_typeCount][Shape::e_typeCount];
	static bool s_initialized;

	uint32 m_flags;

	// World pool and list pointers.
	Contact* m_prev;
	Contact* m_next;

	// Nodes for connecting bodies.
	ContactEdge m_nodeA;
	ContactEdge m_nodeB;

	Fixture* m_fixtureA;
	Fixture* m_fixtureB;

	int32 m_indexA;
	int32 m_indexB;

	Manifold m_manifold;

	int32 m_toiCount;
	float32 m_toi;

	float32 m_friction;
	float32 m_restitution;

	float32 m_tangentSpeed;
};

inline Manifold* Contact::GetManifold()
{
	return &m_manifold;
}

inline const Manifold* Contact::GetManifold() const
{
	return &m_manifold;
}

inline void Contact::GetWorldManifold(WorldManifold* worldManifold) const
{
	const Body* bodyA = m_fixtureA->GetBody();
	const Body* bodyB = m_fixtureB->GetBody();
	const Shape* shapeA = m_fixtureA->GetShape();
	const Shape* shapeB = m_fixtureB->GetShape();

	worldManifold->Initialize(&m_manifold, bodyA->GetTransform(), shapeA->m_radius, bodyB->GetTransform(), shapeB->m_radius);
}

inline void Contact::SetEnabled(bool flag)
{
	if (flag)
	{
		m_flags |= e_enabledFlag;
	}
	else
	{
		m_flags &= ~e_enabledFlag;
	}
}

inline bool Contact::IsEnabled() const
{
	return (m_flags & e_enabledFlag) == e_enabledFlag;
}

inline bool Contact::IsTouching() const
{
	return (m_flags & e_touchingFlag) == e_touchingFlag;
}

inline Contact* Contact::GetNext()
{
	return m_next;
}

inline const Contact* Contact::GetNext() const
{
	return m_next;
}

inline Fixture* Contact::GetFixtureA()
{
	return m_fixtureA;
}

inline const Fixture* Contact::GetFixtureA() const
{
	return m_fixtureA;
}

inline Fixture* Contact::GetFixtureB()
{
	return m_fixtureB;
}

inline int32 Contact::GetChildIndexA() const
{
	return m_indexA;
}

inline const Fixture* Contact::GetFixtureB() const
{
	return m_fixtureB;
}

inline int32 Contact::GetChildIndexB() const
{
	return m_indexB;
}

inline void Contact::FlagForFiltering()
{
	m_flags |= e_filterFlag;
}

inline void Contact::SetFriction(float32 friction)
{
	m_friction = friction;
}

inline float32 Contact::GetFriction() const
{
	return m_friction;
}

inline void Contact::ResetFriction()
{
	m_friction = MixFriction(m_fixtureA->m_friction, m_fixtureB->m_friction);
}

inline void Contact::SetRestitution(float32 restitution)
{
	m_restitution = restitution;
}

inline float32 Contact::GetRestitution() const
{
	return m_restitution;
}

inline void Contact::ResetRestitution()
{
	m_restitution = MixRestitution(m_fixtureA->m_restitution, m_fixtureB->m_restitution);
}

inline void Contact::SetTangentSpeed(float32 speed)
{
	m_tangentSpeed = speed;
}

inline float32 Contact::GetTangentSpeed() const
{
	return m_tangentSpeed;
}

} // namespace b2

#endif // B2_CONTACT_HPP
