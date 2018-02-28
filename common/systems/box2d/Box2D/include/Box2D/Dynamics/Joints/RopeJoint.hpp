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

#ifndef B2_ROPE_JOINT_HPP
#define B2_ROPE_JOINT_HPP

#include <Box2D/Dynamics/Joints/Joint.hpp>

namespace b2
{

/// Rope joint definition. This requires two body anchor points and
/// a maximum lengths.
/// Note: by default the connected objects will not collide.
/// see collideConnected in b2::JointDef.
struct RopeJointDef : public JointDef
{
	RopeJointDef()
	{
		type = e_ropeJoint;
		localAnchorA.Set(-1.0f, 0.0f);
		localAnchorB.Set(1.0f, 0.0f);
		maxLength = 0.0f;
	}

	/// The local anchor point relative to bodyA's origin.
	Vec2 localAnchorA;

	/// The local anchor point relative to bodyB's origin.
	Vec2 localAnchorB;

	/// The maximum length of the rope.
	/// Warning: this must be larger than linearSlop or
	/// the joint will have no effect.
	float32 maxLength;
};

/// A rope joint enforces a maximum distance between two points
/// on two bodies. It has no other effect.
/// Warning: if you attempt to change the maximum length during
/// the simulation you will get some non-physical behavior.
/// A model that would allow you to dynamically modify the length
/// would have some sponginess, so I chose not to implement it
/// that way. See b2::DistanceJoint if you want to dynamically
/// control length.
class RopeJoint : public Joint
{
public:
	Vec2 GetAnchorA() const;
	Vec2 GetAnchorB() const;

	Vec2 GetReactionForce(float32 inv_dt) const;
	float32 GetReactionTorque(float32 inv_dt) const;

	/// The local anchor point relative to bodyA's origin.
	const Vec2& GetLocalAnchorA() const { return m_localAnchorA; }

	/// The local anchor point relative to bodyB's origin.
	const Vec2& GetLocalAnchorB() const  { return m_localAnchorB; }

	/// Set/Get the maximum length of the rope.
	void SetMaxLength(float32 length) { m_maxLength = length; }
	float32 GetMaxLength() const;

	LimitState GetLimitState() const;

	/// Dump joint to dmLog
	void Dump();

protected:

	friend class Joint;
	RopeJoint(const RopeJointDef* data);

	void InitVelocityConstraints(const SolverData& data);
	void SolveVelocityConstraints(const SolverData& data);
	bool SolvePositionConstraints(const SolverData& data);

	// Solver shared
	Vec2 m_localAnchorA;
	Vec2 m_localAnchorB;
	float32 m_maxLength;
	float32 m_length;
	float32 m_impulse;

	// Solver temp
	int32 m_indexA;
	int32 m_indexB;
	Vec2 m_u;
	Vec2 m_rA;
	Vec2 m_rB;
	Vec2 m_localCenterA;
	Vec2 m_localCenterB;
	float32 m_invMassA;
	float32 m_invMassB;
	float32 m_invIA;
	float32 m_invIB;
	float32 m_mass;
	LimitState m_state;
};

} // namespace b2

#endif // B2_ROPE_JOINT_HPP
