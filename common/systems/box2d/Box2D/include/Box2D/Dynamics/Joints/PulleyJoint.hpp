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

#ifndef B2_PULLEY_JOINT_HPP
#define B2_PULLEY_JOINT_HPP

#include <Box2D/Dynamics/Joints/Joint.hpp>

namespace b2
{

const float32 minPulleyLength = 2.0f;

/// Pulley joint definition. This requires two ground anchors,
/// two dynamic body anchor points, and a pulley ratio.
struct PulleyJointDef : public JointDef
{
	PulleyJointDef()
	{
		type = e_pulleyJoint;
		groundAnchorA.Set(-1.0f, 1.0f);
		groundAnchorB.Set(1.0f, 1.0f);
		localAnchorA.Set(-1.0f, 0.0f);
		localAnchorB.Set(1.0f, 0.0f);
		lengthA = 0.0f;
		lengthB = 0.0f;
		ratio = 1.0f;
		collideConnected = true;
	}

	/// Initialize the bodies, anchors, lengths, max lengths, and ratio using the world anchors.
	void Initialize(Body* bodyA, Body* bodyB,
					const Vec2& groundAnchorA, const Vec2& groundAnchorB,
					const Vec2& anchorA, const Vec2& anchorB,
					float32 ratio);

	/// The first ground anchor in world coordinates. This point never moves.
	Vec2 groundAnchorA;

	/// The second ground anchor in world coordinates. This point never moves.
	Vec2 groundAnchorB;

	/// The local anchor point relative to bodyA's origin.
	Vec2 localAnchorA;

	/// The local anchor point relative to bodyB's origin.
	Vec2 localAnchorB;

	/// The a reference length for the segment attached to bodyA.
	float32 lengthA;

	/// The a reference length for the segment attached to bodyB.
	float32 lengthB;

	/// The pulley ratio, used to simulate a block-and-tackle.
	float32 ratio;
};

/// The pulley joint is connected to two bodies and two fixed ground points.
/// The pulley supports a ratio such that:
/// length1 + ratio * length2 <= constant
/// Yes, the force transmitted is scaled by the ratio.
/// Warning: the pulley joint can get a bit squirrelly by itself. They often
/// work better when combined with prismatic joints. You should also cover the
/// the anchor points with static shapes to prevent one side from going to
/// zero length.
class PulleyJoint : public Joint
{
public:
	Vec2 GetAnchorA() const;
	Vec2 GetAnchorB() const;

	Vec2 GetReactionForce(float32 inv_dt) const;
	float32 GetReactionTorque(float32 inv_dt) const;

	/// Get the first ground anchor.
	Vec2 GetGroundAnchorA() const;

	/// Get the second ground anchor.
	Vec2 GetGroundAnchorB() const;

	/// Get the current length of the segment attached to bodyA.
	float32 GetLengthA() const;

	/// Get the current length of the segment attached to bodyB.
	float32 GetLengthB() const;

	/// Get the pulley ratio.
	float32 GetRatio() const;

	/// Get the current length of the segment attached to bodyA.
	float32 GetCurrentLengthA() const;

	/// Get the current length of the segment attached to bodyB.
	float32 GetCurrentLengthB() const;

	/// Dump joint to dmLog
	void Dump();

	/// Implement b2::Joint::ShiftOrigin
	void ShiftOrigin(const Vec2& newOrigin);

protected:

	friend class Joint;
	PulleyJoint(const PulleyJointDef* data);

	void InitVelocityConstraints(const SolverData& data);
	void SolveVelocityConstraints(const SolverData& data);
	bool SolvePositionConstraints(const SolverData& data);

	Vec2 m_groundAnchorA;
	Vec2 m_groundAnchorB;
	float32 m_lengthA;
	float32 m_lengthB;
	
	// Solver shared
	Vec2 m_localAnchorA;
	Vec2 m_localAnchorB;
	float32 m_constant;
	float32 m_ratio;
	float32 m_impulse;

	// Solver temp
	int32 m_indexA;
	int32 m_indexB;
	Vec2 m_uA;
	Vec2 m_uB;
	Vec2 m_rA;
	Vec2 m_rB;
	Vec2 m_localCenterA;
	Vec2 m_localCenterB;
	float32 m_invMassA;
	float32 m_invMassB;
	float32 m_invIA;
	float32 m_invIB;
	float32 m_mass;
};

} // namespace b2

#endif // B2_PULLEY_JOINT_HPP
