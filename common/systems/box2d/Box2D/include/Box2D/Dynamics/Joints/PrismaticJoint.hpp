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

#ifndef B2_PRISMATIC_JOINT_HPP
#define B2_PRISMATIC_JOINT_HPP

#include <Box2D/Dynamics/Joints/Joint.hpp>

namespace b2
{

/// Prismatic joint definition. This requires defining a line of
/// motion using an axis and an anchor point. The definition uses local
/// anchor points and a local axis so that the initial configuration
/// can violate the constraint slightly. The joint translation is zero
/// when the local anchor points coincide in world space. Using local
/// anchors and a local axis helps when saving and loading a game.
struct PrismaticJointDef : public JointDef
{
	PrismaticJointDef()
	{
		type = e_prismaticJoint;
		localAnchorA.SetZero();
		localAnchorB.SetZero();
		localAxisA.Set(1.0f, 0.0f);
		referenceAngle = 0.0f;
		enableLimit = false;
		lowerTranslation = 0.0f;
		upperTranslation = 0.0f;
		enableMotor = false;
		maxMotorForce = 0.0f;
		motorSpeed = 0.0f;
	}

	/// Initialize the bodies, anchors, axis, and reference angle using the world
	/// anchor and unit world axis.
	void Initialize(Body* bodyA, Body* bodyB, const Vec2& anchor, const Vec2& axis);

	/// The local anchor point relative to bodyA's origin.
	Vec2 localAnchorA;

	/// The local anchor point relative to bodyB's origin.
	Vec2 localAnchorB;

	/// The local translation unit axis in bodyA.
	Vec2 localAxisA;

	/// The constrained angle between the bodies: bodyB_angle - bodyA_angle.
	float32 referenceAngle;

	/// Enable/disable the joint limit.
	bool enableLimit;

	/// The lower translation limit, usually in meters.
	float32 lowerTranslation;

	/// The upper translation limit, usually in meters.
	float32 upperTranslation;

	/// Enable/disable the joint motor.
	bool enableMotor;

	/// The maximum motor torque, usually in N-m.
	float32 maxMotorForce;

	/// The desired motor speed in radians per second.
	float32 motorSpeed;
};

/// A prismatic joint. This joint provides one degree of freedom: translation
/// along an axis fixed in bodyA. Relative rotation is prevented. You can
/// use a joint limit to restrict the range of motion and a joint motor to
/// drive the motion or to model joint friction.
class PrismaticJoint : public Joint
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

	/// The local joint axis relative to bodyA.
	const Vec2& GetLocalAxisA() const { return m_localXAxisA; }

	/// Get the reference angle.
	float32 GetReferenceAngle() const { return m_referenceAngle; }

	/// Get the current joint translation, usually in meters.
	float32 GetJointTranslation() const;

	/// Get the current joint translation speed, usually in meters per second.
	float32 GetJointSpeed() const;

	/// Is the joint limit enabled?
	bool IsLimitEnabled() const;

	/// Enable/disable the joint limit.
	void EnableLimit(bool flag);

	/// Get the lower joint limit, usually in meters.
	float32 GetLowerLimit() const;

	/// Get the upper joint limit, usually in meters.
	float32 GetUpperLimit() const;

	/// Set the joint limits, usually in meters.
	void SetLimits(float32 lower, float32 upper);

	/// Is the joint motor enabled?
	bool IsMotorEnabled() const;

	/// Enable/disable the joint motor.
	void EnableMotor(bool flag);

	/// Set the motor speed, usually in meters per second.
	void SetMotorSpeed(float32 speed);

	/// Get the motor speed, usually in meters per second.
	float32 GetMotorSpeed() const;

	/// Set the maximum motor force, usually in N.
	void SetMaxMotorForce(float32 force);
	float32 GetMaxMotorForce() const { return m_maxMotorForce; }

	/// Get the current motor force given the inverse time step, usually in N.
	float32 GetMotorForce(float32 inv_dt) const;

	/// Dump to b2::Log
	void Dump();

protected:
	friend class Joint;
	friend class GearJoint;
	PrismaticJoint(const PrismaticJointDef* def);

	void InitVelocityConstraints(const SolverData& data);
	void SolveVelocityConstraints(const SolverData& data);
	bool SolvePositionConstraints(const SolverData& data);

	// Solver shared
	Vec2 m_localAnchorA;
	Vec2 m_localAnchorB;
	Vec2 m_localXAxisA;
	Vec2 m_localYAxisA;
	float32 m_referenceAngle;
	Vec3 m_impulse;
	float32 m_motorImpulse;
	float32 m_lowerTranslation;
	float32 m_upperTranslation;
	float32 m_maxMotorForce;
	float32 m_motorSpeed;
	bool m_enableLimit;
	bool m_enableMotor;
	LimitState m_limitState;

	// Solver temp
	int32 m_indexA;
	int32 m_indexB;
	Vec2 m_localCenterA;
	Vec2 m_localCenterB;
	float32 m_invMassA;
	float32 m_invMassB;
	float32 m_invIA;
	float32 m_invIB;
	Vec2 m_axis, m_perp;
	float32 m_s1, m_s2;
	float32 m_a1, m_a2;
	Mat33 m_K;
	float32 m_motorMass;
};

inline float32 PrismaticJoint::GetMotorSpeed() const
{
	return m_motorSpeed;
}

} // namespace b2

#endif // B2_PRISMATIC_JOINT_HPP
