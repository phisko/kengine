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

#ifndef B2_WELD_JOINT_HPP
#define B2_WELD_JOINT_HPP

#include <Box2D/Dynamics/Joints/Joint.hpp>

namespace b2
{

/// Weld joint definition. You need to specify local anchor points
/// where they are attached and the relative body angle. The position
/// of the anchor points is important for computing the reaction torque.
struct WeldJointDef : public JointDef
{
	WeldJointDef()
	{
		type = e_weldJoint;
		localAnchorA.Set(0.0f, 0.0f);
		localAnchorB.Set(0.0f, 0.0f);
		referenceAngle = 0.0f;
		frequencyHz = 0.0f;
		dampingRatio = 0.0f;
	}

	/// Initialize the bodies, anchors, and reference angle using a world
	/// anchor point.
	void Initialize(Body* bodyA, Body* bodyB, const Vec2& anchor);

	/// The local anchor point relative to bodyA's origin.
	Vec2 localAnchorA;

	/// The local anchor point relative to bodyB's origin.
	Vec2 localAnchorB;

	/// The bodyB angle minus bodyA angle in the reference state (radians).
	float32 referenceAngle;
	
	/// The mass-spring-damper frequency in Hertz. Rotation only.
	/// Disable softness with a value of 0.
	float32 frequencyHz;

	/// The damping ratio. 0 = no damping, 1 = critical damping.
	float32 dampingRatio;
};

/// A weld joint essentially glues two bodies together. A weld joint may
/// distort somewhat because the island constraint solver is approximate.
class WeldJoint : public Joint
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

	/// Get the reference angle.
	float32 GetReferenceAngle() const { return m_referenceAngle; }

	/// Set/get frequency in Hz.
	void SetFrequency(float32 hz) { m_frequencyHz = hz; }
	float32 GetFrequency() const { return m_frequencyHz; }

	/// Set/get damping ratio.
	void SetDampingRatio(float32 ratio) { m_dampingRatio = ratio; }
	float32 GetDampingRatio() const { return m_dampingRatio; }

	/// Dump to b2::Log
	void Dump();

protected:

	friend class Joint;

	WeldJoint(const WeldJointDef* def);

	void InitVelocityConstraints(const SolverData& data);
	void SolveVelocityConstraints(const SolverData& data);
	bool SolvePositionConstraints(const SolverData& data);

	float32 m_frequencyHz;
	float32 m_dampingRatio;
	float32 m_bias;

	// Solver shared
	Vec2 m_localAnchorA;
	Vec2 m_localAnchorB;
	float32 m_referenceAngle;
	float32 m_gamma;
	Vec3 m_impulse;

	// Solver temp
	int32 m_indexA;
	int32 m_indexB;
	Vec2 m_rA;
	Vec2 m_rB;
	Vec2 m_localCenterA;
	Vec2 m_localCenterB;
	float32 m_invMassA;
	float32 m_invMassB;
	float32 m_invIA;
	float32 m_invIB;
	Mat33 m_mass;
};

} // namespace b2

#endif // B2_WELD_JOINT_HPP
