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

#ifndef B2_JOINT_HPP
#define B2_JOINT_HPP

#include <Box2D/Common/Math.hpp>

namespace b2
{

class Body;
class Joint;
struct SolverData;
class BlockAllocator;

enum JointType
{
	e_unknownJoint,
	e_revoluteJoint,
	e_prismaticJoint,
	e_distanceJoint,
	e_pulleyJoint,
	e_mouseJoint,
	e_gearJoint,
	e_wheelJoint,
    e_weldJoint,
	e_frictionJoint,
	e_ropeJoint,
	e_motorJoint
};

enum LimitState
{
	e_inactiveLimit,
	e_atLowerLimit,
	e_atUpperLimit,
	e_equalLimits
};

struct Jacobian
{
	Vec2 linear;
	float32 angularA;
	float32 angularB;
};

/// A joint edge is used to connect bodies and joints together
/// in a joint graph where each body is a node and each joint
/// is an edge. A joint edge belongs to a doubly linked list
/// maintained in each attached body. Each joint has two joint
/// nodes, one for each attached body.
struct JointEdge
{
	Body* other;			///< provides quick access to the other body attached.
	Joint* joint;			///< the joint
	JointEdge* prev;		///< the previous joint edge in the body's joint list
	JointEdge* next;		///< the next joint edge in the body's joint list
};

/// Joint definitions are used to construct joints.
struct JointDef
{
	JointDef()
	{
		type = e_unknownJoint;
		userData = NULL;
		bodyA = NULL;
		bodyB = NULL;
		collideConnected = false;
	}

	/// The joint type is set automatically for concrete joint types.
	JointType type;

	/// Use this to attach application specific data to your joints.
	void* userData;

	/// The first attached body.
	Body* bodyA;

	/// The second attached body.
	Body* bodyB;

	/// Set this flag to true if the attached bodies should collide.
	bool collideConnected;
};

/// The base joint class. Joints are used to constraint two bodies together in
/// various fashions. Some joints also feature limits and motors.
class Joint
{
public:

	/// Get the type of the concrete joint.
	JointType GetType() const;

	/// Get the first body attached to this joint.
	Body* GetBodyA();

	/// Get the second body attached to this joint.
	Body* GetBodyB();

	/// Get the anchor point on bodyA in world coordinates.
	virtual Vec2 GetAnchorA() const = 0;

	/// Get the anchor point on bodyB in world coordinates.
	virtual Vec2 GetAnchorB() const = 0;

	/// Get the reaction force on bodyB at the joint anchor in Newtons.
	virtual Vec2 GetReactionForce(float32 inv_dt) const = 0;

	/// Get the reaction torque on bodyB in N*m.
	virtual float32 GetReactionTorque(float32 inv_dt) const = 0;

	/// Get the next joint the world joint list.
	Joint* GetNext();
	const Joint* GetNext() const;

	/// Get the user data pointer.
	void* GetUserData() const;

	/// Set the user data pointer.
	void SetUserData(void* data);

	/// Short-cut function to determine if either body is inactive.
	bool IsActive() const;

	/// Get collide connected.
	/// Note: modifying the collide connect flag won't work correctly because
	/// the flag is only checked when fixture AABBs begin to overlap.
	bool GetCollideConnected() const;

	/// Dump this joint to the log file.
	virtual void Dump() { Log("// Dump is not supported for this joint type.\n"); }

	/// Shift the origin for any points stored in world coordinates.
	virtual void ShiftOrigin(const Vec2& newOrigin) { B2_NOT_USED(newOrigin);  }

protected:
	friend class World;
	friend class Body;
	friend class Island;
	friend class GearJoint;

	static Joint* Create(const JointDef* def, BlockAllocator* allocator);
	static void Destroy(Joint* joint, BlockAllocator* allocator);

	Joint(const JointDef* def);
	virtual ~Joint() {}

	virtual void InitVelocityConstraints(const SolverData& data) = 0;
	virtual void SolveVelocityConstraints(const SolverData& data) = 0;

	// This returns true if the position errors are within tolerance.
	virtual bool SolvePositionConstraints(const SolverData& data) = 0;

	JointType m_type;
	Joint* m_prev;
	Joint* m_next;
	JointEdge m_edgeA;
	JointEdge m_edgeB;
	Body* m_bodyA;
	Body* m_bodyB;

	int32 m_index;

	bool m_islandFlag;
	bool m_collideConnected;

	void* m_userData;
};

inline JointType Joint::GetType() const
{
	return m_type;
}

inline Body* Joint::GetBodyA()
{
	return m_bodyA;
}

inline Body* Joint::GetBodyB()
{
	return m_bodyB;
}

inline Joint* Joint::GetNext()
{
	return m_next;
}

inline const Joint* Joint::GetNext() const
{
	return m_next;
}

inline void* Joint::GetUserData() const
{
	return m_userData;
}

inline void Joint::SetUserData(void* data)
{
	m_userData = data;
}

inline bool Joint::GetCollideConnected() const
{
	return m_collideConnected;
}

} // namespace b2

#endif // B2_JOINT_HPP
