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

#ifndef B2_BODY_HPP
#define B2_BODY_HPP

#include <Box2D/Common/Math.hpp>
#include <Box2D/Collision/Shapes/Shape.hpp>
#include <memory>

namespace b2
{

class Fixture;
class Joint;
class Contact;
class Controller;
class World;
struct FixtureDef;
struct JointEdge;
struct ContactEdge;

/// The body type.
/// static: zero mass, zero velocity, may be manually moved
/// kinematic: zero mass, non-zero velocity set by user, moved by solver
/// dynamic: positive mass, non-zero velocity determined by forces, moved by solver
enum BodyType
{
	staticBody = 0,
	kinematicBody,
	dynamicBody

	// TODO_ERIN
	//bulletBody,
};

/// A body definition holds all the data needed to construct a rigid body.
/// You can safely re-use body definitions. Shapes are added to a body after construction.
struct BodyDef
{
	/// This constructor sets the body definition default values.
	BodyDef()
	{
		userData = NULL;
		position.Set(0.0f, 0.0f);
		angle = 0.0f;
		linearVelocity.Set(0.0f, 0.0f);
		angularVelocity = 0.0f;
		linearDamping = 0.0f;
		angularDamping = 0.0f;
		allowSleep = true;
		awake = true;
		fixedRotation = false;
		bullet = false;
		type = staticBody;
		active = true;
		gravityScale = 1.0f;
	}

	/// The body type: static, kinematic, or dynamic.
	/// Note: if a dynamic body would have zero mass, the mass is set to one.
	BodyType type;

	/// The world position of the body. Avoid creating bodies at the origin
	/// since this can lead to many overlapping shapes.
	Vec2 position;

	/// The world angle of the body in radians.
	float32 angle;

	/// The linear velocity of the body's origin in world co-ordinates.
	Vec2 linearVelocity;

	/// The angular velocity of the body.
	float32 angularVelocity;

	/// Linear damping is use to reduce the linear velocity. The damping parameter
	/// can be larger than 1.0f but the damping effect becomes sensitive to the
	/// time step when the damping parameter is large.
	float32 linearDamping;

	/// Angular damping is use to reduce the angular velocity. The damping parameter
	/// can be larger than 1.0f but the damping effect becomes sensitive to the
	/// time step when the damping parameter is large.
	float32 angularDamping;

	/// Set this flag to false if this body should never fall asleep. Note that
	/// this increases CPU usage.
	bool allowSleep;

	/// Is this body initially awake or sleeping?
	bool awake;

	/// Should this body be prevented from rotating? Useful for characters.
	bool fixedRotation;

	/// Is this a fast moving body that should be prevented from tunneling through
	/// other moving bodies? Note that all bodies are prevented from tunneling through
	/// kinematic and static bodies. This setting is only considered on dynamic bodies.
	/// @warning You should use this flag sparingly since it increases processing time.
	bool bullet;

	/// Does this body start out active?
	bool active;

	/// Use this to store application specific body data.
	void* userData;

	/// Scale the gravity applied to this body.
	float32 gravityScale;
};

/// A rigid body. These are created via b2::World::CreateBody.
class Body
{
public:
	/// Creates a fixture and attach it to this body. Use this function if you need
	/// to set some fixture parameters, like friction. Otherwise you can create the
	/// fixture directly from a shape.
	/// If the density is non-zero, this function automatically updates the mass of the body.
	/// Contacts are not created until the next time step.
	/// @param def the fixture definition.
	/// @warning This function is locked during callbacks.
	Fixture* CreateFixture(const FixtureDef* def);

	/// Creates a fixture from a shape and attach it to this body.
	/// This is a convenience function. Use b2::FixtureDef if you need to set parameters
	/// like friction, restitution, user data, or filtering.
	/// If the density is non-zero, this function automatically updates the mass of the body.
	/// @param shape the shape to be cloned.
	/// @param density the shape density (set to zero for static bodies).
	/// @warning This function is locked during callbacks.
	Fixture* CreateFixture(const Shape* shape, float32 density);

	/// Destroy a fixture. This removes the fixture from the broad-phase and
	/// destroys all contacts associated with this fixture. This will
	/// automatically adjust the mass of the body if the body is dynamic and the
	/// fixture has positive density.
	/// All fixtures attached to a body are implicitly destroyed when the body is destroyed.
	/// @param fixture the fixture to be removed.
	/// @warning This function is locked during callbacks.
	void DestroyFixture(Fixture* fixture);

	/// Set the position of the body's origin and rotation.
	/// Manipulating a body's transform may cause non-physical behavior.
	/// Note: contacts are updated on the next call to b2::World::Step.
	/// @param position the world position of the body's local origin.
	/// @param angle the world rotation in radians.
	void SetTransform(const Vec2& position, float32 angle);

	/// Get the body transform for the body's origin.
	/// @return the world transform of the body's origin.
	const Transform& GetTransform() const;

	/// Get the world body origin position.
	/// @return the world position of the body's origin.
	const Vec2& GetPosition() const;

	/// Get the angle in radians.
	/// @return the current world rotation angle in radians.
	float32 GetAngle() const;

	/// Get the world position of the center of mass.
	const Vec2& GetWorldCenter() const;

	/// Get the local position of the center of mass.
	const Vec2& GetLocalCenter() const;

	/// Set the linear velocity of the center of mass.
	/// @param v the new linear velocity of the center of mass.
	void SetLinearVelocity(const Vec2& v);

	/// Get the linear velocity of the center of mass.
	/// @return the linear velocity of the center of mass.
	const Vec2& GetLinearVelocity() const;

	/// Set the angular velocity.
	/// @param omega the new angular velocity in radians/second.
	void SetAngularVelocity(float32 omega);

	/// Get the angular velocity.
	/// @return the angular velocity in radians/second.
	float32 GetAngularVelocity() const;

	/// Apply a force at a world point. If the force is not
	/// applied at the center of mass, it will generate a torque and
	/// affect the angular velocity. This wakes up the body.
	/// @param force the world force vector, usually in Newtons (N).
	/// @param point the world position of the point of application.
	/// @param wake also wake up the body
	void ApplyForce(const Vec2& force, const Vec2& point, bool wake);

	/// Apply a force to the center of mass. This wakes up the body.
	/// @param force the world force vector, usually in Newtons (N).
	/// @param wake also wake up the body
	void ApplyForceToCenter(const Vec2& force, bool wake);

	/// Apply a torque. This affects the angular velocity
	/// without affecting the linear velocity of the center of mass.
	/// This wakes up the body.
	/// @param torque about the z-axis (out of the screen), usually in N-m.
	/// @param wake also wake up the body
	void ApplyTorque(float32 torque, bool wake);

	/// Apply an impulse at a point. This immediately modifies the velocity.
	/// It also modifies the angular velocity if the point of application
	/// is not at the center of mass. This wakes up the body.
	/// @param impulse the world impulse vector, usually in N-seconds or kg-m/s.
	/// @param point the world position of the point of application.
	/// @param wake also wake up the body
	void ApplyLinearImpulse(const Vec2& impulse, const Vec2& point, bool wake);

	/// Apply an angular impulse.
	/// @param impulse the angular impulse in units of kg*m*m/s
	/// @param wake also wake up the body
	void ApplyAngularImpulse(float32 impulse, bool wake);

	/// Get the total mass of the body.
	/// @return the mass, usually in kilograms (kg).
	float32 GetMass() const;

	/// Get the rotational inertia of the body about the local origin.
	/// @return the rotational inertia, usually in kg-m^2.
	float32 GetInertia() const;

	/// Get the mass data of the body.
	/// @return a struct containing the mass, inertia and center of the body.
	void GetMassData(MassData* data) const;

	/// Set the mass properties to override the mass properties of the fixtures.
	/// Note that this changes the center of mass position.
	/// Note that creating or destroying fixtures can also alter the mass.
	/// This function has no effect if the body isn't dynamic.
	/// @param massData the mass properties.
	void SetMassData(const MassData* data);

	/// This resets the mass properties to the sum of the mass properties of the fixtures.
	/// This normally does not need to be called unless you called SetMassData to override
	/// the mass and you later want to reset the mass.
	void ResetMassData();

	/// Get the world coordinates of a point given the local coordinates.
	/// @param localPoint a point on the body measured relative the the body's origin.
	/// @return the same point expressed in world coordinates.
	Vec2 GetWorldPoint(const Vec2& localPoint) const;

	/// Get the world coordinates of a vector given the local coordinates.
	/// @param localVector a vector fixed in the body.
	/// @return the same vector expressed in world coordinates.
	Vec2 GetWorldVector(const Vec2& localVector) const;

	/// Gets a local point relative to the body's origin given a world point.
	/// @param a point in world coordinates.
	/// @return the corresponding local point relative to the body's origin.
	Vec2 GetLocalPoint(const Vec2& worldPoint) const;

	/// Gets a local vector given a world vector.
	/// @param a vector in world coordinates.
	/// @return the corresponding local vector.
	Vec2 GetLocalVector(const Vec2& worldVector) const;

	/// Get the world linear velocity of a world point attached to this body.
	/// @param a point in world coordinates.
	/// @return the world velocity of a point.
	Vec2 GetLinearVelocityFromWorldPoint(const Vec2& worldPoint) const;

	/// Get the world velocity of a local point.
	/// @param a point in local coordinates.
	/// @return the world velocity of a point.
	Vec2 GetLinearVelocityFromLocalPoint(const Vec2& localPoint) const;

	/// Get the linear damping of the body.
	float32 GetLinearDamping() const;

	/// Set the linear damping of the body.
	void SetLinearDamping(float32 linearDamping);

	/// Get the angular damping of the body.
	float32 GetAngularDamping() const;

	/// Set the angular damping of the body.
	void SetAngularDamping(float32 angularDamping);

	/// Get the gravity scale of the body.
	float32 GetGravityScale() const;

	/// Set the gravity scale of the body.
	void SetGravityScale(float32 scale);

	/// Set the type of this body. This may alter the mass and velocity.
	void SetType(BodyType type);

	/// Get the type of this body.
	BodyType GetType() const;

	/// Should this body be treated like a bullet for continuous collision detection?
	void SetBullet(bool flag);

	/// Is this body treated like a bullet for continuous collision detection?
	bool IsBullet() const;

	/// You can disable sleeping on this body. If you disable sleeping, the
	/// body will be woken.
	void SetSleepingAllowed(bool flag);

	/// Is this body allowed to sleep
	bool IsSleepingAllowed() const;

	/// Set the sleep state of the body. A sleeping body has very
	/// low CPU cost.
	/// @param flag set to true to wake the body, false to put it to sleep.
	void SetAwake(bool flag);

	/// Get the sleeping state of this body.
	/// @return true if the body is awake.
	bool IsAwake() const;

	/// Set the active state of the body. An inactive body is not
	/// simulated and cannot be collided with or woken up.
	/// If you pass a flag of true, all fixtures will be added to the
	/// broad-phase.
	/// If you pass a flag of false, all fixtures will be removed from
	/// the broad-phase and all contacts will be destroyed.
	/// Fixtures and joints are otherwise unaffected. You may continue
	/// to create/destroy fixtures and joints on inactive bodies.
	/// Fixtures on an inactive body are implicitly inactive and will
	/// not participate in collisions, ray-casts, or queries.
	/// Joints connected to an inactive body are implicitly inactive.
	/// An inactive body is still owned by a b2::World object and remains
	/// in the body list.
	void SetActive(bool flag);

	/// Get the active state of the body.
	bool IsActive() const;

	/// Set this body to have fixed rotation. This causes the mass
	/// to be reset.
	void SetFixedRotation(bool flag);

	/// Does this body have fixed rotation?
	bool IsFixedRotation() const;

	/// Get the list of all fixtures attached to this body.
	Fixture* GetFixtureList();
	const Fixture* GetFixtureList() const;

	/// Get the list of all joints attached to this body.
	JointEdge* GetJointList();
	const JointEdge* GetJointList() const;

	/// Get the list of all contacts attached to this body.
	/// @warning this list changes during the time step and you may
	/// miss some collisions if you don't use b2::ContactListener.
	ContactEdge* GetContactList();
	const ContactEdge* GetContactList() const;

	/// Get the next body in the world's body list.
	Body* GetNext();
	const Body* GetNext() const;

	/// Get the user data pointer that was provided in the body definition.
	void* GetUserData() const;

	/// Set the user data. Use this to store your application specific data.
	void SetUserData(void* data);

	/// Get the parent world of this body.
	World* GetWorld();
	const World* GetWorld() const;

	/// Dump this body to a log file
	void Dump();

private:

	friend class World;
	friend class Island;
	friend class ContactManager;
	friend class ContactSolver;
	friend class Contact;
	
	friend class DistanceJoint;
	friend class FrictionJoint;
	friend class GearJoint;
	friend class MotorJoint;
	friend class MouseJoint;
	friend class PrismaticJoint;
	friend class PulleyJoint;
	friend class RevoluteJoint;
	friend class RopeJoint;
	friend class WeldJoint;
	friend class WheelJoint;

	// m_flags
	enum
	{
		e_islandFlag		= 0x0001,
		e_awakeFlag			= 0x0002,
		e_autoSleepFlag		= 0x0004,
		e_bulletFlag		= 0x0008,
		e_fixedRotationFlag	= 0x0010,
		e_activeFlag		= 0x0020,
		e_toiFlag			= 0x0040
	};

	Body(const BodyDef* bd, World* world);
	~Body();

	void SynchronizeFixtures();
	void SynchronizeTransform();

	// This is used to prevent connected bodies from colliding.
	// It may lie, depending on the collideConnected flag.
	bool ShouldCollide(const Body* other) const;

	void Advance(float32 t);

	BodyType m_type;

	uint16 m_flags;

	int32 m_islandIndex;

	Transform m_xf;		// the body origin transform
	Sweep m_sweep;		// the swept motion for CCD

	Vec2 m_linearVelocity;
	float32 m_angularVelocity;

	Vec2 m_force;
	float32 m_torque;

	World* m_world;
	Body* m_prev;
	Body* m_next;

	Fixture* m_fixtureList;
	int32 m_fixtureCount;

	JointEdge* m_jointList;
	ContactEdge* m_contactList;

	float32 m_mass, m_invMass;

	// Rotational inertia about the center of mass.
	float32 m_I, m_invI;

	float32 m_linearDamping;
	float32 m_angularDamping;
	float32 m_gravityScale;

	float32 m_sleepTime;

	void* m_userData;
};

inline BodyType Body::GetType() const
{
	return m_type;
}

inline const Transform& Body::GetTransform() const
{
	return m_xf;
}

inline const Vec2& Body::GetPosition() const
{
	return m_xf.p;
}

inline float32 Body::GetAngle() const
{
	return m_sweep.a;
}

inline const Vec2& Body::GetWorldCenter() const
{
	return m_sweep.c;
}

inline const Vec2& Body::GetLocalCenter() const
{
	return m_sweep.localCenter;
}

inline void Body::SetLinearVelocity(const Vec2& v)
{
	if (m_type == staticBody)
	{
		return;
	}

	if (Dot(v,v) > 0.0f)
	{
		SetAwake(true);
	}

	m_linearVelocity = v;
}

inline const Vec2& Body::GetLinearVelocity() const
{
	return m_linearVelocity;
}

inline void Body::SetAngularVelocity(float32 w)
{
	if (m_type == staticBody)
	{
		return;
	}

	if (w * w > 0.0f)
	{
		SetAwake(true);
	}

	m_angularVelocity = w;
}

inline float32 Body::GetAngularVelocity() const
{
	return m_angularVelocity;
}

inline float32 Body::GetMass() const
{
	return m_mass;
}

inline float32 Body::GetInertia() const
{
	return m_I + m_mass * Dot(m_sweep.localCenter, m_sweep.localCenter);
}

inline void Body::GetMassData(MassData* data) const
{
	data->mass = m_mass;
	data->I = m_I + m_mass * Dot(m_sweep.localCenter, m_sweep.localCenter);
	data->center = m_sweep.localCenter;
}

inline Vec2 Body::GetWorldPoint(const Vec2& localPoint) const
{
	return Mul(m_xf, localPoint);
}

inline Vec2 Body::GetWorldVector(const Vec2& localVector) const
{
	return Mul(m_xf.q, localVector);
}

inline Vec2 Body::GetLocalPoint(const Vec2& worldPoint) const
{
	return MulT(m_xf, worldPoint);
}

inline Vec2 Body::GetLocalVector(const Vec2& worldVector) const
{
	return MulT(m_xf.q, worldVector);
}

inline Vec2 Body::GetLinearVelocityFromWorldPoint(const Vec2& worldPoint) const
{
	return m_linearVelocity + Cross(m_angularVelocity, worldPoint - m_sweep.c);
}

inline Vec2 Body::GetLinearVelocityFromLocalPoint(const Vec2& localPoint) const
{
	return GetLinearVelocityFromWorldPoint(GetWorldPoint(localPoint));
}

inline float32 Body::GetLinearDamping() const
{
	return m_linearDamping;
}

inline void Body::SetLinearDamping(float32 linearDamping)
{
	m_linearDamping = linearDamping;
}

inline float32 Body::GetAngularDamping() const
{
	return m_angularDamping;
}

inline void Body::SetAngularDamping(float32 angularDamping)
{
	m_angularDamping = angularDamping;
}

inline float32 Body::GetGravityScale() const
{
	return m_gravityScale;
}

inline void Body::SetGravityScale(float32 scale)
{
	m_gravityScale = scale;
}

inline void Body::SetBullet(bool flag)
{
	if (flag)
	{
		m_flags |= e_bulletFlag;
	}
	else
	{
		m_flags &= ~e_bulletFlag;
	}
}

inline bool Body::IsBullet() const
{
	return (m_flags & e_bulletFlag) == e_bulletFlag;
}

inline void Body::SetAwake(bool flag)
{
	if (flag)
	{
		if ((m_flags & e_awakeFlag) == 0)
		{
			m_flags |= e_awakeFlag;
			m_sleepTime = 0.0f;
		}
	}
	else
	{
		m_flags &= ~e_awakeFlag;
		m_sleepTime = 0.0f;
		m_linearVelocity.SetZero();
		m_angularVelocity = 0.0f;
		m_force.SetZero();
		m_torque = 0.0f;
	}
}

inline bool Body::IsAwake() const
{
	return (m_flags & e_awakeFlag) == e_awakeFlag;
}

inline bool Body::IsActive() const
{
	return (m_flags & e_activeFlag) == e_activeFlag;
}

inline bool Body::IsFixedRotation() const
{
	return (m_flags & e_fixedRotationFlag) == e_fixedRotationFlag;
}

inline void Body::SetSleepingAllowed(bool flag)
{
	if (flag)
	{
		m_flags |= e_autoSleepFlag;
	}
	else
	{
		m_flags &= ~e_autoSleepFlag;
		SetAwake(true);
	}
}

inline bool Body::IsSleepingAllowed() const
{
	return (m_flags & e_autoSleepFlag) == e_autoSleepFlag;
}

inline Fixture* Body::GetFixtureList()
{
	return m_fixtureList;
}

inline const Fixture* Body::GetFixtureList() const
{
	return m_fixtureList;
}

inline JointEdge* Body::GetJointList()
{
	return m_jointList;
}

inline const JointEdge* Body::GetJointList() const
{
	return m_jointList;
}

inline ContactEdge* Body::GetContactList()
{
	return m_contactList;
}

inline const ContactEdge* Body::GetContactList() const
{
	return m_contactList;
}

inline Body* Body::GetNext()
{
	return m_next;
}

inline const Body* Body::GetNext() const
{
	return m_next;
}

inline void Body::SetUserData(void* data)
{
	m_userData = data;
}

inline void* Body::GetUserData() const
{
	return m_userData;
}

inline void Body::ApplyForce(const Vec2& force, const Vec2& point, bool wake)
{
	if (m_type != dynamicBody)
	{
		return;
	}

	if (wake && (m_flags & e_awakeFlag) == 0)
	{
		SetAwake(true);
	}

	// Don't accumulate a force if the body is sleeping.
	if (m_flags & e_awakeFlag)
	{
		m_force += force;
		m_torque += Cross(point - m_sweep.c, force);
	}
}

inline void Body::ApplyForceToCenter(const Vec2& force, bool wake)
{
	if (m_type != dynamicBody)
	{
		return;
	}

	if (wake && (m_flags & e_awakeFlag) == 0)
	{
		SetAwake(true);
	}

	// Don't accumulate a force if the body is sleeping
	if (m_flags & e_awakeFlag)
	{
		m_force += force;
	}
}

inline void Body::ApplyTorque(float32 torque, bool wake)
{
	if (m_type != dynamicBody)
	{
		return;
	}

	if (wake && (m_flags & e_awakeFlag) == 0)
	{
		SetAwake(true);
	}

	// Don't accumulate a force if the body is sleeping
	if (m_flags & e_awakeFlag)
	{
		m_torque += torque;
	}
}

inline void Body::ApplyLinearImpulse(const Vec2& impulse, const Vec2& point, bool wake)
{
	if (m_type != dynamicBody)
	{
		return;
	}

	if (wake && (m_flags & e_awakeFlag) == 0)
	{
		SetAwake(true);
	}

	// Don't accumulate velocity if the body is sleeping
	if (m_flags & e_awakeFlag)
	{
		m_linearVelocity += m_invMass * impulse;
		m_angularVelocity += m_invI * Cross(point - m_sweep.c, impulse);
	}
}

inline void Body::ApplyAngularImpulse(float32 impulse, bool wake)
{
	if (m_type != dynamicBody)
	{
		return;
	}

	if (wake && (m_flags & e_awakeFlag) == 0)
	{
		SetAwake(true);
	}

	// Don't accumulate velocity if the body is sleeping
	if (m_flags & e_awakeFlag)
	{
		m_angularVelocity += m_invI * impulse;
	}
}

inline void Body::SynchronizeTransform()
{
	m_xf.q.Set(m_sweep.a);
	m_xf.p = m_sweep.c - Mul(m_xf.q, m_sweep.localCenter);
}

inline void Body::Advance(float32 alpha)
{
	// Advance to the new safe time. This doesn't sync the broad-phase.
	m_sweep.Advance(alpha);
	m_sweep.c = m_sweep.c0;
	m_sweep.a = m_sweep.a0;
	m_xf.q.Set(m_sweep.a);
	m_xf.p = m_sweep.c - Mul(m_xf.q, m_sweep.localCenter);
}

inline World* Body::GetWorld()
{
	return m_world;
}

inline const World* Body::GetWorld() const
{
	return m_world;
}

} // namespace b2

#endif // B2_BODY_HPP
