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

#include <Box2D/Dynamics/Joints/PrismaticJoint.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/TimeStep.hpp>

namespace b2
{

// Linear constraint (point-to-line)
// d = p2 - p1 = x2 + r2 - x1 - r1
// C = dot(perp, d)
// Cdot = dot(d, cross(w1, perp)) + dot(perp, v2 + cross(w2, r2) - v1 - cross(w1, r1))
//      = -dot(perp, v1) - dot(cross(d + r1, perp), w1) + dot(perp, v2) + dot(cross(r2, perp), v2)
// J = [-perp, -cross(d + r1, perp), perp, cross(r2,perp)]
//
// Angular constraint
// C = a2 - a1 + a_initial
// Cdot = w2 - w1
// J = [0 0 -1 0 0 1]
//
// K = J * invM * JT
//
// J = [-a -s1 a s2]
//     [0  -1  0  1]
// a = perp
// s1 = cross(d + r1, a) = cross(p2 - x1, a)
// s2 = cross(r2, a) = cross(p2 - x2, a)


// Motor/Limit linear constraint
// C = dot(ax1, d)
// Cdot = = -dot(ax1, v1) - dot(cross(d + r1, ax1), w1) + dot(ax1, v2) + dot(cross(r2, ax1), v2)
// J = [-ax1 -cross(d+r1,ax1) ax1 cross(r2,ax1)]

// Block Solver
// We develop a block solver that includes the joint limit. This makes the limit stiff (inelastic) even
// when the mass has poor distribution (leading to large torques about the joint anchor points).
//
// The Jacobian has 3 rows:
// J = [-uT -s1 uT s2] // linear
//     [0   -1   0  1] // angular
//     [-vT -a1 vT a2] // limit
//
// u = perp
// v = axis
// s1 = cross(d + r1, u), s2 = cross(r2, u)
// a1 = cross(d + r1, v), a2 = cross(r2, v)

// M * (v2 - v1) = JT * df
// J * v2 = bias
//
// v2 = v1 + invM * JT * df
// J * (v1 + invM * JT * df) = bias
// K * df = bias - J * v1 = -Cdot
// K = J * invM * JT
// Cdot = J * v1 - bias
//
// Now solve for f2.
// df = f2 - f1
// K * (f2 - f1) = -Cdot
// f2 = invK * (-Cdot) + f1
//
// Clamp accumulated limit impulse.
// lower: f2(3) = max(f2(3), 0)
// upper: f2(3) = min(f2(3), 0)
//
// Solve for correct f2(1:2)
// K(1:2, 1:2) * f2(1:2) = -Cdot(1:2) - K(1:2,3) * f2(3) + K(1:2,1:3) * f1
//                       = -Cdot(1:2) - K(1:2,3) * f2(3) + K(1:2,1:2) * f1(1:2) + K(1:2,3) * f1(3)
// K(1:2, 1:2) * f2(1:2) = -Cdot(1:2) - K(1:2,3) * (f2(3) - f1(3)) + K(1:2,1:2) * f1(1:2)
// f2(1:2) = invK(1:2,1:2) * (-Cdot(1:2) - K(1:2,3) * (f2(3) - f1(3))) + f1(1:2)
//
// Now compute impulse to be applied:
// df = f2 - f1

void PrismaticJointDef::Initialize(Body* bA, Body* bB, const Vec2& anchor, const Vec2& axis)
{
	bodyA = bA;
	bodyB = bB;
	localAnchorA = bodyA->GetLocalPoint(anchor);
	localAnchorB = bodyB->GetLocalPoint(anchor);
	localAxisA = bodyA->GetLocalVector(axis);
	referenceAngle = bodyB->GetAngle() - bodyA->GetAngle();
}

PrismaticJoint::PrismaticJoint(const PrismaticJointDef* def)
: Joint(def)
{
	m_localAnchorA = def->localAnchorA;
	m_localAnchorB = def->localAnchorB;
	m_localXAxisA = def->localAxisA;
	m_localXAxisA.Normalize();
	m_localYAxisA = Cross(1.0f, m_localXAxisA);
	m_referenceAngle = def->referenceAngle;

	m_impulse.SetZero();
	m_motorMass = 0.0f;
	m_motorImpulse = 0.0f;

	m_lowerTranslation = def->lowerTranslation;
	m_upperTranslation = def->upperTranslation;
	m_maxMotorForce = def->maxMotorForce;
	m_motorSpeed = def->motorSpeed;
	m_enableLimit = def->enableLimit;
	m_enableMotor = def->enableMotor;
	m_limitState = e_inactiveLimit;

	m_axis.SetZero();
	m_perp.SetZero();
}

void PrismaticJoint::InitVelocityConstraints(const SolverData& data)
{
	m_indexA = m_bodyA->m_islandIndex;
	m_indexB = m_bodyB->m_islandIndex;
	m_localCenterA = m_bodyA->m_sweep.localCenter;
	m_localCenterB = m_bodyB->m_sweep.localCenter;
	m_invMassA = m_bodyA->m_invMass;
	m_invMassB = m_bodyB->m_invMass;
	m_invIA = m_bodyA->m_invI;
	m_invIB = m_bodyB->m_invI;

	Vec2 cA = data.positions[m_indexA].c;
	float32 aA = data.positions[m_indexA].a;
	Vec2 vA = data.velocities[m_indexA].v;
	float32 wA = data.velocities[m_indexA].w;

	Vec2 cB = data.positions[m_indexB].c;
	float32 aB = data.positions[m_indexB].a;
	Vec2 vB = data.velocities[m_indexB].v;
	float32 wB = data.velocities[m_indexB].w;

	Rot qA(aA), qB(aB);

	// Compute the effective masses.
	Vec2 rA = Mul(qA, m_localAnchorA - m_localCenterA);
	Vec2 rB = Mul(qB, m_localAnchorB - m_localCenterB);
	Vec2 d = (cB - cA) + rB - rA;

	float32 mA = m_invMassA, mB = m_invMassB;
	float32 iA = m_invIA, iB = m_invIB;

	// Compute motor Jacobian and effective mass.
	{
		m_axis = Mul(qA, m_localXAxisA);
		m_a1 = Cross(d + rA, m_axis);
		m_a2 = Cross(rB, m_axis);

		m_motorMass = mA + mB + iA * m_a1 * m_a1 + iB * m_a2 * m_a2;
		if (m_motorMass > 0.0f)
		{
			m_motorMass = 1.0f / m_motorMass;
		}
	}

	// Prismatic constraint.
	{
		m_perp = Mul(qA, m_localYAxisA);

		m_s1 = Cross(d + rA, m_perp);
		m_s2 = Cross(rB, m_perp);

        float32 s1test;
        s1test = Cross(rA, m_perp);

		float32 k11 = mA + mB + iA * m_s1 * m_s1 + iB * m_s2 * m_s2;
		float32 k12 = iA * m_s1 + iB * m_s2;
		float32 k13 = iA * m_s1 * m_a1 + iB * m_s2 * m_a2;
		float32 k22 = iA + iB;
		if (k22 == 0.0f)
		{
			// For bodies with fixed rotation.
			k22 = 1.0f;
		}
		float32 k23 = iA * m_a1 + iB * m_a2;
		float32 k33 = mA + mB + iA * m_a1 * m_a1 + iB * m_a2 * m_a2;

		m_K.ex.Set(k11, k12, k13);
		m_K.ey.Set(k12, k22, k23);
		m_K.ez.Set(k13, k23, k33);
	}

	// Compute motor and limit terms.
	if (m_enableLimit)
	{
		float32 jointTranslation = Dot(m_axis, d);
		if (Abs(m_upperTranslation - m_lowerTranslation) < 2.0f * linearSlop)
		{
			m_limitState = e_equalLimits;
		}
		else if (jointTranslation <= m_lowerTranslation)
		{
			if (m_limitState != e_atLowerLimit)
			{
				m_limitState = e_atLowerLimit;
				m_impulse.z = 0.0f;
			}
		}
		else if (jointTranslation >= m_upperTranslation)
		{
			if (m_limitState != e_atUpperLimit)
			{
				m_limitState = e_atUpperLimit;
				m_impulse.z = 0.0f;
			}
		}
		else
		{
			m_limitState = e_inactiveLimit;
			m_impulse.z = 0.0f;
		}
	}
	else
	{
		m_limitState = e_inactiveLimit;
		m_impulse.z = 0.0f;
	}

	if (m_enableMotor == false)
	{
		m_motorImpulse = 0.0f;
	}

	if (data.step.warmStarting)
	{
		// Account for variable time step.
		m_impulse *= data.step.dtRatio;
		m_motorImpulse *= data.step.dtRatio;

		Vec2 P = m_impulse.x * m_perp + (m_motorImpulse + m_impulse.z) * m_axis;
		float32 LA = m_impulse.x * m_s1 + m_impulse.y + (m_motorImpulse + m_impulse.z) * m_a1;
		float32 LB = m_impulse.x * m_s2 + m_impulse.y + (m_motorImpulse + m_impulse.z) * m_a2;

		vA -= mA * P;
		wA -= iA * LA;

		vB += mB * P;
		wB += iB * LB;
	}
	else
	{
		m_impulse.SetZero();
		m_motorImpulse = 0.0f;
	}

	data.velocities[m_indexA].v = vA;
	data.velocities[m_indexA].w = wA;
	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

void PrismaticJoint::SolveVelocityConstraints(const SolverData& data)
{
	Vec2 vA = data.velocities[m_indexA].v;
	float32 wA = data.velocities[m_indexA].w;
	Vec2 vB = data.velocities[m_indexB].v;
	float32 wB = data.velocities[m_indexB].w;

	float32 mA = m_invMassA, mB = m_invMassB;
	float32 iA = m_invIA, iB = m_invIB;

	// Solve linear motor constraint.
	if (m_enableMotor && m_limitState != e_equalLimits)
	{
		float32 Cdot = Dot(m_axis, vB - vA) + m_a2 * wB - m_a1 * wA;
		float32 impulse = m_motorMass * (m_motorSpeed - Cdot);
		float32 oldImpulse = m_motorImpulse;
		float32 maxImpulse = data.step.dt * m_maxMotorForce;
		m_motorImpulse = Clamp(m_motorImpulse + impulse, -maxImpulse, maxImpulse);
		impulse = m_motorImpulse - oldImpulse;

		Vec2 P = impulse * m_axis;
		float32 LA = impulse * m_a1;
		float32 LB = impulse * m_a2;

		vA -= mA * P;
		wA -= iA * LA;

		vB += mB * P;
		wB += iB * LB;
	}

	Vec2 Cdot1;
	Cdot1.x = Dot(m_perp, vB - vA) + m_s2 * wB - m_s1 * wA;
	Cdot1.y = wB - wA;

	if (m_enableLimit && m_limitState != e_inactiveLimit)
	{
		// Solve prismatic and limit constraint in block form.
		float32 Cdot2;
		Cdot2 = Dot(m_axis, vB - vA) + m_a2 * wB - m_a1 * wA;
		Vec3 Cdot(Cdot1.x, Cdot1.y, Cdot2);

		Vec3 f1 = m_impulse;
		Vec3 df =  m_K.Solve33(-Cdot);
		m_impulse += df;

		if (m_limitState == e_atLowerLimit)
		{
			m_impulse.z = Max(m_impulse.z, 0.0f);
		}
		else if (m_limitState == e_atUpperLimit)
		{
			m_impulse.z = Min(m_impulse.z, 0.0f);
		}

		// f2(1:2) = invK(1:2,1:2) * (-Cdot(1:2) - K(1:2,3) * (f2(3) - f1(3))) + f1(1:2)
		Vec2 b = -Cdot1 - (m_impulse.z - f1.z) * Vec2(m_K.ez.x, m_K.ez.y);
		Vec2 f2r = m_K.Solve22(b) + Vec2(f1.x, f1.y);
		m_impulse.x = f2r.x;
		m_impulse.y = f2r.y;

		df = m_impulse - f1;

		Vec2 P = df.x * m_perp + df.z * m_axis;
		float32 LA = df.x * m_s1 + df.y + df.z * m_a1;
		float32 LB = df.x * m_s2 + df.y + df.z * m_a2;

		vA -= mA * P;
		wA -= iA * LA;

		vB += mB * P;
		wB += iB * LB;
	}
	else
	{
		// Limit is inactive, just solve the prismatic constraint in block form.
		Vec2 df = m_K.Solve22(-Cdot1);
		m_impulse.x += df.x;
		m_impulse.y += df.y;

		Vec2 P = df.x * m_perp;
		float32 LA = df.x * m_s1 + df.y;
		float32 LB = df.x * m_s2 + df.y;

		vA -= mA * P;
		wA -= iA * LA;

		vB += mB * P;
		wB += iB * LB;
	}

	data.velocities[m_indexA].v = vA;
	data.velocities[m_indexA].w = wA;
	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

bool PrismaticJoint::SolvePositionConstraints(const SolverData& data)
{
	Vec2 cA = data.positions[m_indexA].c;
	float32 aA = data.positions[m_indexA].a;
	Vec2 cB = data.positions[m_indexB].c;
	float32 aB = data.positions[m_indexB].a;

	Rot qA(aA), qB(aB);

	float32 mA = m_invMassA, mB = m_invMassB;
	float32 iA = m_invIA, iB = m_invIB;

	// Compute fresh Jacobians
	Vec2 rA = Mul(qA, m_localAnchorA - m_localCenterA);
	Vec2 rB = Mul(qB, m_localAnchorB - m_localCenterB);
	Vec2 d = cB + rB - cA - rA;

	Vec2 axis = Mul(qA, m_localXAxisA);
	float32 a1 = Cross(d + rA, axis);
	float32 a2 = Cross(rB, axis);
	Vec2 perp = Mul(qA, m_localYAxisA);

	float32 s1 = Cross(d + rA, perp);
	float32 s2 = Cross(rB, perp);

	Vec3 impulse;
	Vec2 C1;
	C1.x = Dot(perp, d);
	C1.y = aB - aA - m_referenceAngle;

	float32 linearError = Abs(C1.x);
	float32 angularError = Abs(C1.y);

	bool active = false;
	float32 C2 = 0.0f;
	if (m_enableLimit)
	{
		float32 translation = Dot(axis, d);
		if (Abs(m_upperTranslation - m_lowerTranslation) < 2.0f * linearSlop)
		{
			// Prevent large angular corrections
			C2 = Clamp(translation, -maxLinearCorrection, maxLinearCorrection);
			linearError = Max(linearError, Abs(translation));
			active = true;
		}
		else if (translation <= m_lowerTranslation)
		{
			// Prevent large linear corrections and allow some slop.
			C2 = Clamp(translation - m_lowerTranslation + linearSlop, -maxLinearCorrection, 0.0f);
			linearError = Max(linearError, m_lowerTranslation - translation);
			active = true;
		}
		else if (translation >= m_upperTranslation)
		{
			// Prevent large linear corrections and allow some slop.
			C2 = Clamp(translation - m_upperTranslation - linearSlop, 0.0f, maxLinearCorrection);
			linearError = Max(linearError, translation - m_upperTranslation);
			active = true;
		}
	}

	if (active)
	{
		float32 k11 = mA + mB + iA * s1 * s1 + iB * s2 * s2;
		float32 k12 = iA * s1 + iB * s2;
		float32 k13 = iA * s1 * a1 + iB * s2 * a2;
		float32 k22 = iA + iB;
		if (k22 == 0.0f)
		{
			// For fixed rotation
			k22 = 1.0f;
		}
		float32 k23 = iA * a1 + iB * a2;
		float32 k33 = mA + mB + iA * a1 * a1 + iB * a2 * a2;

		Mat33 K;
		K.ex.Set(k11, k12, k13);
		K.ey.Set(k12, k22, k23);
		K.ez.Set(k13, k23, k33);

		Vec3 C;
		C.x = C1.x;
		C.y = C1.y;
		C.z = C2;

		impulse = K.Solve33(-C);
	}
	else
	{
		float32 k11 = mA + mB + iA * s1 * s1 + iB * s2 * s2;
		float32 k12 = iA * s1 + iB * s2;
		float32 k22 = iA + iB;
		if (k22 == 0.0f)
		{
			k22 = 1.0f;
		}

		Mat22 K;
		K.ex.Set(k11, k12);
		K.ey.Set(k12, k22);

		Vec2 impulse1 = K.Solve(-C1);
		impulse.x = impulse1.x;
		impulse.y = impulse1.y;
		impulse.z = 0.0f;
	}

	Vec2 P = impulse.x * perp + impulse.z * axis;
	float32 LA = impulse.x * s1 + impulse.y + impulse.z * a1;
	float32 LB = impulse.x * s2 + impulse.y + impulse.z * a2;

	cA -= mA * P;
	aA -= iA * LA;
	cB += mB * P;
	aB += iB * LB;

	data.positions[m_indexA].c = cA;
	data.positions[m_indexA].a = aA;
	data.positions[m_indexB].c = cB;
	data.positions[m_indexB].a = aB;

	return linearError <= linearSlop && angularError <= angularSlop;
}

Vec2 PrismaticJoint::GetAnchorA() const
{
	return m_bodyA->GetWorldPoint(m_localAnchorA);
}

Vec2 PrismaticJoint::GetAnchorB() const
{
	return m_bodyB->GetWorldPoint(m_localAnchorB);
}

Vec2 PrismaticJoint::GetReactionForce(float32 inv_dt) const
{
	return inv_dt * (m_impulse.x * m_perp + (m_motorImpulse + m_impulse.z) * m_axis);
}

float32 PrismaticJoint::GetReactionTorque(float32 inv_dt) const
{
	return inv_dt * m_impulse.y;
}

float32 PrismaticJoint::GetJointTranslation() const
{
	Vec2 pA = m_bodyA->GetWorldPoint(m_localAnchorA);
	Vec2 pB = m_bodyB->GetWorldPoint(m_localAnchorB);
	Vec2 d = pB - pA;
	Vec2 axis = m_bodyA->GetWorldVector(m_localXAxisA);

	float32 translation = Dot(d, axis);
	return translation;
}

float32 PrismaticJoint::GetJointSpeed() const
{
	Body* bA = m_bodyA;
	Body* bB = m_bodyB;

	Vec2 rA = Mul(bA->m_xf.q, m_localAnchorA - bA->m_sweep.localCenter);
	Vec2 rB = Mul(bB->m_xf.q, m_localAnchorB - bB->m_sweep.localCenter);
	Vec2 p1 = bA->m_sweep.c + rA;
	Vec2 p2 = bB->m_sweep.c + rB;
	Vec2 d = p2 - p1;
	Vec2 axis = Mul(bA->m_xf.q, m_localXAxisA);

	Vec2 vA = bA->m_linearVelocity;
	Vec2 vB = bB->m_linearVelocity;
	float32 wA = bA->m_angularVelocity;
	float32 wB = bB->m_angularVelocity;

	float32 speed = Dot(d, Cross(wA, axis)) + Dot(axis, vB + Cross(wB, rB) - vA - Cross(wA, rA));
	return speed;
}

bool PrismaticJoint::IsLimitEnabled() const
{
	return m_enableLimit;
}

void PrismaticJoint::EnableLimit(bool flag)
{
	if (flag != m_enableLimit)
	{
		m_bodyA->SetAwake(true);
		m_bodyB->SetAwake(true);
		m_enableLimit = flag;
		m_impulse.z = 0.0f;
	}
}

float32 PrismaticJoint::GetLowerLimit() const
{
	return m_lowerTranslation;
}

float32 PrismaticJoint::GetUpperLimit() const
{
	return m_upperTranslation;
}

void PrismaticJoint::SetLimits(float32 lower, float32 upper)
{
	assert(lower <= upper);
	if (lower != m_lowerTranslation || upper != m_upperTranslation)
	{
		m_bodyA->SetAwake(true);
		m_bodyB->SetAwake(true);
		m_lowerTranslation = lower;
		m_upperTranslation = upper;
		m_impulse.z = 0.0f;
	}
}

bool PrismaticJoint::IsMotorEnabled() const
{
	return m_enableMotor;
}

void PrismaticJoint::EnableMotor(bool flag)
{
	m_bodyA->SetAwake(true);
	m_bodyB->SetAwake(true);
	m_enableMotor = flag;
}

void PrismaticJoint::SetMotorSpeed(float32 speed)
{
	m_bodyA->SetAwake(true);
	m_bodyB->SetAwake(true);
	m_motorSpeed = speed;
}

void PrismaticJoint::SetMaxMotorForce(float32 force)
{
	m_bodyA->SetAwake(true);
	m_bodyB->SetAwake(true);
	m_maxMotorForce = force;
}

float32 PrismaticJoint::GetMotorForce(float32 inv_dt) const
{
	return inv_dt * m_motorImpulse;
}

void PrismaticJoint::Dump()
{
	int32 indexA = m_bodyA->m_islandIndex;
	int32 indexB = m_bodyB->m_islandIndex;

	Log("  b2::PrismaticJointDef jd;\n");
	Log("  jd.bodyA = bodies[%d];\n", indexA);
	Log("  jd.bodyB = bodies[%d];\n", indexB);
	Log("  jd.collideConnected = bool(%d);\n", m_collideConnected);
	Log("  jd.localAnchorA.Set(%.15lef, %.15lef);\n", m_localAnchorA.x, m_localAnchorA.y);
	Log("  jd.localAnchorB.Set(%.15lef, %.15lef);\n", m_localAnchorB.x, m_localAnchorB.y);
	Log("  jd.localAxisA.Set(%.15lef, %.15lef);\n", m_localXAxisA.x, m_localXAxisA.y);
	Log("  jd.referenceAngle = %.15lef;\n", m_referenceAngle);
	Log("  jd.enableLimit = bool(%d);\n", m_enableLimit);
	Log("  jd.lowerTranslation = %.15lef;\n", m_lowerTranslation);
	Log("  jd.upperTranslation = %.15lef;\n", m_upperTranslation);
	Log("  jd.enableMotor = bool(%d);\n", m_enableMotor);
	Log("  jd.motorSpeed = %.15lef;\n", m_motorSpeed);
	Log("  jd.maxMotorForce = %.15lef;\n", m_maxMotorForce);
	Log("  joints[%d] = m_world->CreateJoint(&jd);\n", m_index);
}

} // namespace b2
