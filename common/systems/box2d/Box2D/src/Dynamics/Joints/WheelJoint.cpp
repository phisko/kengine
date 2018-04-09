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

#include <Box2D/Dynamics/Joints/WheelJoint.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/TimeStep.hpp>

namespace b2
{

// Linear constraint (point-to-line)
// d = pB - pA = xB + rB - xA - rA
// C = dot(ay, d)
// Cdot = dot(d, cross(wA, ay)) + dot(ay, vB + cross(wB, rB) - vA - cross(wA, rA))
//      = -dot(ay, vA) - dot(cross(d + rA, ay), wA) + dot(ay, vB) + dot(cross(rB, ay), vB)
// J = [-ay, -cross(d + rA, ay), ay, cross(rB, ay)]

// Spring linear constraint
// C = dot(ax, d)
// Cdot = = -dot(ax, vA) - dot(cross(d + rA, ax), wA) + dot(ax, vB) + dot(cross(rB, ax), vB)
// J = [-ax -cross(d+rA, ax) ax cross(rB, ax)]

// Motor rotational constraint
// Cdot = wB - wA
// J = [0 0 -1 0 0 1]

void WheelJointDef::Initialize(Body* bA, Body* bB, const Vec2& anchor, const Vec2& axis)
{
	bodyA = bA;
	bodyB = bB;
	localAnchorA = bodyA->GetLocalPoint(anchor);
	localAnchorB = bodyB->GetLocalPoint(anchor);
	localAxisA = bodyA->GetLocalVector(axis);
}

WheelJoint::WheelJoint(const WheelJointDef* def)
: Joint(def)
{
	m_localAnchorA = def->localAnchorA;
	m_localAnchorB = def->localAnchorB;
	m_localXAxisA = def->localAxisA;
	m_localYAxisA = Cross(1.0f, m_localXAxisA);

	m_mass = 0.0f;
	m_impulse = 0.0f;
	m_motorMass = 0.0f;
	m_motorImpulse = 0.0f;
	m_springMass = 0.0f;
	m_springImpulse = 0.0f;

	m_maxMotorTorque = def->maxMotorTorque;
	m_motorSpeed = def->motorSpeed;
	m_enableMotor = def->enableMotor;

	m_frequencyHz = def->frequencyHz;
	m_dampingRatio = def->dampingRatio;

	m_bias = 0.0f;
	m_gamma = 0.0f;

	m_ax.SetZero();
	m_ay.SetZero();
}

void WheelJoint::InitVelocityConstraints(const SolverData& data)
{
	m_indexA = m_bodyA->m_islandIndex;
	m_indexB = m_bodyB->m_islandIndex;
	m_localCenterA = m_bodyA->m_sweep.localCenter;
	m_localCenterB = m_bodyB->m_sweep.localCenter;
	m_invMassA = m_bodyA->m_invMass;
	m_invMassB = m_bodyB->m_invMass;
	m_invIA = m_bodyA->m_invI;
	m_invIB = m_bodyB->m_invI;

	float32 mA = m_invMassA, mB = m_invMassB;
	float32 iA = m_invIA, iB = m_invIB;

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
	Vec2 d = cB + rB - cA - rA;

	// Point to line constraint
	{
		m_ay = Mul(qA, m_localYAxisA);
		m_sAy = Cross(d + rA, m_ay);
		m_sBy = Cross(rB, m_ay);

		m_mass = mA + mB + iA * m_sAy * m_sAy + iB * m_sBy * m_sBy;

		if (m_mass > 0.0f)
		{
			m_mass = 1.0f / m_mass;
		}
	}

	// Spring constraint
	m_springMass = 0.0f;
	m_bias = 0.0f;
	m_gamma = 0.0f;
	if (m_frequencyHz > 0.0f)
	{
		m_ax = Mul(qA, m_localXAxisA);
		m_sAx = Cross(d + rA, m_ax);
		m_sBx = Cross(rB, m_ax);

		float32 invMass = mA + mB + iA * m_sAx * m_sAx + iB * m_sBx * m_sBx;

		if (invMass > 0.0f)
		{
			m_springMass = 1.0f / invMass;

			float32 C = Dot(d, m_ax);

			// Frequency
			float32 omega = 2.0f * pi * m_frequencyHz;

			// Damping coefficient
			float32 d = 2.0f * m_springMass * m_dampingRatio * omega;

			// Spring stiffness
			float32 k = m_springMass * omega * omega;

			// magic formulas
			float32 h = data.step.dt;
			m_gamma = h * (d + h * k);
			if (m_gamma > 0.0f)
			{
				m_gamma = 1.0f / m_gamma;
			}

			m_bias = C * h * k * m_gamma;

			m_springMass = invMass + m_gamma;
			if (m_springMass > 0.0f)
			{
				m_springMass = 1.0f / m_springMass;
			}
		}
	}
	else
	{
		m_springImpulse = 0.0f;
	}

	// Rotational motor
	if (m_enableMotor)
	{
		m_motorMass = iA + iB;
		if (m_motorMass > 0.0f)
		{
			m_motorMass = 1.0f / m_motorMass;
		}
	}
	else
	{
		m_motorMass = 0.0f;
		m_motorImpulse = 0.0f;
	}

	if (data.step.warmStarting)
	{
		// Account for variable time step.
		m_impulse *= data.step.dtRatio;
		m_springImpulse *= data.step.dtRatio;
		m_motorImpulse *= data.step.dtRatio;

		Vec2 P = m_impulse * m_ay + m_springImpulse * m_ax;
		float32 LA = m_impulse * m_sAy + m_springImpulse * m_sAx + m_motorImpulse;
		float32 LB = m_impulse * m_sBy + m_springImpulse * m_sBx + m_motorImpulse;

		vA -= m_invMassA * P;
		wA -= m_invIA * LA;

		vB += m_invMassB * P;
		wB += m_invIB * LB;
	}
	else
	{
		m_impulse = 0.0f;
		m_springImpulse = 0.0f;
		m_motorImpulse = 0.0f;
	}

	data.velocities[m_indexA].v = vA;
	data.velocities[m_indexA].w = wA;
	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

void WheelJoint::SolveVelocityConstraints(const SolverData& data)
{
	float32 mA = m_invMassA, mB = m_invMassB;
	float32 iA = m_invIA, iB = m_invIB;

	Vec2 vA = data.velocities[m_indexA].v;
	float32 wA = data.velocities[m_indexA].w;
	Vec2 vB = data.velocities[m_indexB].v;
	float32 wB = data.velocities[m_indexB].w;

	// Solve spring constraint
	{
		float32 Cdot = Dot(m_ax, vB - vA) + m_sBx * wB - m_sAx * wA;
		float32 impulse = -m_springMass * (Cdot + m_bias + m_gamma * m_springImpulse);
		m_springImpulse += impulse;

		Vec2 P = impulse * m_ax;
		float32 LA = impulse * m_sAx;
		float32 LB = impulse * m_sBx;

		vA -= mA * P;
		wA -= iA * LA;

		vB += mB * P;
		wB += iB * LB;
	}

	// Solve rotational motor constraint
	{
		float32 Cdot = wB - wA - m_motorSpeed;
		float32 impulse = -m_motorMass * Cdot;

		float32 oldImpulse = m_motorImpulse;
		float32 maxImpulse = data.step.dt * m_maxMotorTorque;
		m_motorImpulse = Clamp(m_motorImpulse + impulse, -maxImpulse, maxImpulse);
		impulse = m_motorImpulse - oldImpulse;

		wA -= iA * impulse;
		wB += iB * impulse;
	}

	// Solve point to line constraint
	{
		float32 Cdot = Dot(m_ay, vB - vA) + m_sBy * wB - m_sAy * wA;
		float32 impulse = -m_mass * Cdot;
		m_impulse += impulse;

		Vec2 P = impulse * m_ay;
		float32 LA = impulse * m_sAy;
		float32 LB = impulse * m_sBy;

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

bool WheelJoint::SolvePositionConstraints(const SolverData& data)
{
	Vec2 cA = data.positions[m_indexA].c;
	float32 aA = data.positions[m_indexA].a;
	Vec2 cB = data.positions[m_indexB].c;
	float32 aB = data.positions[m_indexB].a;

	Rot qA(aA), qB(aB);

	Vec2 rA = Mul(qA, m_localAnchorA - m_localCenterA);
	Vec2 rB = Mul(qB, m_localAnchorB - m_localCenterB);
	Vec2 d = (cB - cA) + rB - rA;

	Vec2 ay = Mul(qA, m_localYAxisA);

	float32 sAy = Cross(d + rA, ay);
	float32 sBy = Cross(rB, ay);

	float32 C = Dot(d, ay);

	float32 k = m_invMassA + m_invMassB + m_invIA * m_sAy * m_sAy + m_invIB * m_sBy * m_sBy;

	float32 impulse;
	if (k != 0.0f)
	{
		impulse = - C / k;
	}
	else
	{
		impulse = 0.0f;
	}

	Vec2 P = impulse * ay;
	float32 LA = impulse * sAy;
	float32 LB = impulse * sBy;

	cA -= m_invMassA * P;
	aA -= m_invIA * LA;
	cB += m_invMassB * P;
	aB += m_invIB * LB;

	data.positions[m_indexA].c = cA;
	data.positions[m_indexA].a = aA;
	data.positions[m_indexB].c = cB;
	data.positions[m_indexB].a = aB;

	return Abs(C) <= linearSlop;
}

Vec2 WheelJoint::GetAnchorA() const
{
	return m_bodyA->GetWorldPoint(m_localAnchorA);
}

Vec2 WheelJoint::GetAnchorB() const
{
	return m_bodyB->GetWorldPoint(m_localAnchorB);
}

Vec2 WheelJoint::GetReactionForce(float32 inv_dt) const
{
	return inv_dt * (m_impulse * m_ay + m_springImpulse * m_ax);
}

float32 WheelJoint::GetReactionTorque(float32 inv_dt) const
{
	return inv_dt * m_motorImpulse;
}

float32 WheelJoint::GetJointTranslation() const
{
	Body* bA = m_bodyA;
	Body* bB = m_bodyB;

	Vec2 pA = bA->GetWorldPoint(m_localAnchorA);
	Vec2 pB = bB->GetWorldPoint(m_localAnchorB);
	Vec2 d = pB - pA;
	Vec2 axis = bA->GetWorldVector(m_localXAxisA);

	float32 translation = Dot(d, axis);
	return translation;
}

float32 WheelJoint::GetJointSpeed() const
{
	float32 wA = m_bodyA->m_angularVelocity;
	float32 wB = m_bodyB->m_angularVelocity;
	return wB - wA;
}

bool WheelJoint::IsMotorEnabled() const
{
	return m_enableMotor;
}

void WheelJoint::EnableMotor(bool flag)
{
	m_bodyA->SetAwake(true);
	m_bodyB->SetAwake(true);
	m_enableMotor = flag;
}

void WheelJoint::SetMotorSpeed(float32 speed)
{
	m_bodyA->SetAwake(true);
	m_bodyB->SetAwake(true);
	m_motorSpeed = speed;
}

void WheelJoint::SetMaxMotorTorque(float32 torque)
{
	m_bodyA->SetAwake(true);
	m_bodyB->SetAwake(true);
	m_maxMotorTorque = torque;
}

float32 WheelJoint::GetMotorTorque(float32 inv_dt) const
{
	return inv_dt * m_motorImpulse;
}

void WheelJoint::Dump()
{
	int32 indexA = m_bodyA->m_islandIndex;
	int32 indexB = m_bodyB->m_islandIndex;

	Log("  b2::WheelJointDef jd;\n");
	Log("  jd.bodyA = bodies[%d];\n", indexA);
	Log("  jd.bodyB = bodies[%d];\n", indexB);
	Log("  jd.collideConnected = bool(%d);\n", m_collideConnected);
	Log("  jd.localAnchorA.Set(%.15lef, %.15lef);\n", m_localAnchorA.x, m_localAnchorA.y);
	Log("  jd.localAnchorB.Set(%.15lef, %.15lef);\n", m_localAnchorB.x, m_localAnchorB.y);
	Log("  jd.localAxisA.Set(%.15lef, %.15lef);\n", m_localXAxisA.x, m_localXAxisA.y);
	Log("  jd.enableMotor = bool(%d);\n", m_enableMotor);
	Log("  jd.motorSpeed = %.15lef;\n", m_motorSpeed);
	Log("  jd.maxMotorTorque = %.15lef;\n", m_maxMotorTorque);
	Log("  jd.frequencyHz = %.15lef;\n", m_frequencyHz);
	Log("  jd.dampingRatio = %.15lef;\n", m_dampingRatio);
	Log("  joints[%d] = m_world->CreateJoint(&jd);\n", m_index);
}

} // namespace b2
