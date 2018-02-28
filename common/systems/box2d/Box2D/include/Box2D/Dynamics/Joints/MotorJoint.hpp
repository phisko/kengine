/*
* Copyright (c) 2006-2012 Erin Catto http://www.box2d.org
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

#ifndef B2_MOTOR_JOINT_HPP
#define B2_MOTOR_JOINT_HPP

#include <Box2D/Dynamics/Joints/Joint.hpp>

namespace b2
{

/// Motor joint definition.
struct MotorJointDef : public JointDef
{
	MotorJointDef()
	{
		type = e_motorJoint;
		linearOffset.SetZero();
		angularOffset = 0.0f;
		maxForce = 1.0f;
		maxTorque = 1.0f;
		correctionFactor = 0.3f;
	}

	/// Initialize the bodies and offsets using the current transforms.
	void Initialize(Body* bodyA, Body* bodyB);

	/// Position of bodyB minus the position of bodyA, in bodyA's frame, in meters.
	Vec2 linearOffset;

	/// The bodyB angle minus bodyA angle in radians.
	float32 angularOffset;
	
	/// The maximum motor force in N.
	float32 maxForce;

	/// The maximum motor torque in N-m.
	float32 maxTorque;

	/// Position correction factor in the range [0,1].
	float32 correctionFactor;
};

/// A motor joint is used to control the relative motion
/// between two bodies. A typical usage is to control the movement
/// of a dynamic body with respect to the ground.
class MotorJoint : public Joint
{
public:
	Vec2 GetAnchorA() const;
	Vec2 GetAnchorB() const;

	Vec2 GetReactionForce(float32 inv_dt) const;
	float32 GetReactionTorque(float32 inv_dt) const;

	/// Set/get the target linear offset, in frame A, in meters.
	void SetLinearOffset(const Vec2& linearOffset);
	const Vec2& GetLinearOffset() const;

	/// Set/get the target angular offset, in radians.
	void SetAngularOffset(float32 angularOffset);
	float32 GetAngularOffset() const;

	/// Set the maximum friction force in N.
	void SetMaxForce(float32 force);

	/// Get the maximum friction force in N.
	float32 GetMaxForce() const;

	/// Set the maximum friction torque in N*m.
	void SetMaxTorque(float32 torque);

	/// Get the maximum friction torque in N*m.
	float32 GetMaxTorque() const;

	/// Set the position correction factor in the range [0,1].
	void SetCorrectionFactor(float32 factor);

	/// Get the position correction factor in the range [0,1].
	float32 GetCorrectionFactor() const;

	/// Dump to b2::Log
	void Dump();

protected:

	friend class Joint;

	MotorJoint(const MotorJointDef* def);

	void InitVelocityConstraints(const SolverData& data);
	void SolveVelocityConstraints(const SolverData& data);
	bool SolvePositionConstraints(const SolverData& data);

	// Solver shared
	Vec2 m_linearOffset;
	float32 m_angularOffset;
	Vec2 m_linearImpulse;
	float32 m_angularImpulse;
	float32 m_maxForce;
	float32 m_maxTorque;
	float32 m_correctionFactor;

	// Solver temp
	int32 m_indexA;
	int32 m_indexB;
	Vec2 m_rA;
	Vec2 m_rB;
	Vec2 m_localCenterA;
	Vec2 m_localCenterB;
	Vec2 m_linearError;
	float32 m_angularError;
	float32 m_invMassA;
	float32 m_invMassB;
	float32 m_invIA;
	float32 m_invIB;
	Mat22 m_linearMass;
	float32 m_angularMass;
};

} // namespace b2

#endif // B2_MOTOR_JOINT_HPP
