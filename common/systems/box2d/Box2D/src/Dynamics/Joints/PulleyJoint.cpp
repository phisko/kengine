/*
* Copyright (c) 2007 Erin Catto http://www.box2d.org
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

#include <Box2D/Dynamics/Joints/PulleyJoint.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/TimeStep.hpp>

namespace b2
{

// Pulley:
// length1 = norm(p1 - s1)
// length2 = norm(p2 - s2)
// C0 = (length1 + ratio * length2)_initial
// C = C0 - (length1 + ratio * length2)
// u1 = (p1 - s1) / norm(p1 - s1)
// u2 = (p2 - s2) / norm(p2 - s2)
// Cdot = -dot(u1, v1 + cross(w1, r1)) - ratio * dot(u2, v2 + cross(w2, r2))
// J = -[u1 cross(r1, u1) ratio * u2  ratio * cross(r2, u2)]
// K = J * invM * JT
//   = invMass1 + invI1 * cross(r1, u1)^2 + ratio^2 * (invMass2 + invI2 * cross(r2, u2)^2)

void PulleyJointDef::Initialize(Body* bA, Body* bB,
				const Vec2& groundA, const Vec2& groundB,
				const Vec2& anchorA, const Vec2& anchorB,
				float32 r)
{
	bodyA = bA;
	bodyB = bB;
	groundAnchorA = groundA;
	groundAnchorB = groundB;
	localAnchorA = bodyA->GetLocalPoint(anchorA);
	localAnchorB = bodyB->GetLocalPoint(anchorB);
	Vec2 dA = anchorA - groundA;
	lengthA = dA.Length();
	Vec2 dB = anchorB - groundB;
	lengthB = dB.Length();
	ratio = r;
	assert(ratio > epsilon);
}

PulleyJoint::PulleyJoint(const PulleyJointDef* def)
: Joint(def)
{
	m_groundAnchorA = def->groundAnchorA;
	m_groundAnchorB = def->groundAnchorB;
	m_localAnchorA = def->localAnchorA;
	m_localAnchorB = def->localAnchorB;

	m_lengthA = def->lengthA;
	m_lengthB = def->lengthB;

	assert(def->ratio != 0.0f);
	m_ratio = def->ratio;

	m_constant = def->lengthA + m_ratio * def->lengthB;

	m_impulse = 0.0f;
}

void PulleyJoint::InitVelocityConstraints(const SolverData& data)
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

	m_rA = Mul(qA, m_localAnchorA - m_localCenterA);
	m_rB = Mul(qB, m_localAnchorB - m_localCenterB);

	// Get the pulley axes.
	m_uA = cA + m_rA - m_groundAnchorA;
	m_uB = cB + m_rB - m_groundAnchorB;

	float32 lengthA = m_uA.Length();
	float32 lengthB = m_uB.Length();

	if (lengthA > 10.0f * linearSlop)
	{
		m_uA *= 1.0f / lengthA;
	}
	else
	{
		m_uA.SetZero();
	}

	if (lengthB > 10.0f * linearSlop)
	{
		m_uB *= 1.0f / lengthB;
	}
	else
	{
		m_uB.SetZero();
	}

	// Compute effective mass.
	float32 ruA = Cross(m_rA, m_uA);
	float32 ruB = Cross(m_rB, m_uB);

	float32 mA = m_invMassA + m_invIA * ruA * ruA;
	float32 mB = m_invMassB + m_invIB * ruB * ruB;

	m_mass = mA + m_ratio * m_ratio * mB;

	if (m_mass > 0.0f)
	{
		m_mass = 1.0f / m_mass;
	}

	if (data.step.warmStarting)
	{
		// Scale impulses to support variable time steps.
		m_impulse *= data.step.dtRatio;

		// Warm starting.
		Vec2 PA = -(m_impulse) * m_uA;
		Vec2 PB = (-m_ratio * m_impulse) * m_uB;

		vA += m_invMassA * PA;
		wA += m_invIA * Cross(m_rA, PA);
		vB += m_invMassB * PB;
		wB += m_invIB * Cross(m_rB, PB);
	}
	else
	{
		m_impulse = 0.0f;
	}

	data.velocities[m_indexA].v = vA;
	data.velocities[m_indexA].w = wA;
	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

void PulleyJoint::SolveVelocityConstraints(const SolverData& data)
{
	Vec2 vA = data.velocities[m_indexA].v;
	float32 wA = data.velocities[m_indexA].w;
	Vec2 vB = data.velocities[m_indexB].v;
	float32 wB = data.velocities[m_indexB].w;

	Vec2 vpA = vA + Cross(wA, m_rA);
	Vec2 vpB = vB + Cross(wB, m_rB);

	float32 Cdot = -Dot(m_uA, vpA) - m_ratio * Dot(m_uB, vpB);
	float32 impulse = -m_mass * Cdot;
	m_impulse += impulse;

	Vec2 PA = -impulse * m_uA;
	Vec2 PB = -m_ratio * impulse * m_uB;
	vA += m_invMassA * PA;
	wA += m_invIA * Cross(m_rA, PA);
	vB += m_invMassB * PB;
	wB += m_invIB * Cross(m_rB, PB);

	data.velocities[m_indexA].v = vA;
	data.velocities[m_indexA].w = wA;
	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

bool PulleyJoint::SolvePositionConstraints(const SolverData& data)
{
	Vec2 cA = data.positions[m_indexA].c;
	float32 aA = data.positions[m_indexA].a;
	Vec2 cB = data.positions[m_indexB].c;
	float32 aB = data.positions[m_indexB].a;

	Rot qA(aA), qB(aB);

	Vec2 rA = Mul(qA, m_localAnchorA - m_localCenterA);
	Vec2 rB = Mul(qB, m_localAnchorB - m_localCenterB);

	// Get the pulley axes.
	Vec2 uA = cA + rA - m_groundAnchorA;
	Vec2 uB = cB + rB - m_groundAnchorB;

	float32 lengthA = uA.Length();
	float32 lengthB = uB.Length();

	if (lengthA > 10.0f * linearSlop)
	{
		uA *= 1.0f / lengthA;
	}
	else
	{
		uA.SetZero();
	}

	if (lengthB > 10.0f * linearSlop)
	{
		uB *= 1.0f / lengthB;
	}
	else
	{
		uB.SetZero();
	}

	// Compute effective mass.
	float32 ruA = Cross(rA, uA);
	float32 ruB = Cross(rB, uB);

	float32 mA = m_invMassA + m_invIA * ruA * ruA;
	float32 mB = m_invMassB + m_invIB * ruB * ruB;

	float32 mass = mA + m_ratio * m_ratio * mB;

	if (mass > 0.0f)
	{
		mass = 1.0f / mass;
	}

	float32 C = m_constant - lengthA - m_ratio * lengthB;
	float32 linearError = Abs(C);

	float32 impulse = -mass * C;

	Vec2 PA = -impulse * uA;
	Vec2 PB = -m_ratio * impulse * uB;

	cA += m_invMassA * PA;
	aA += m_invIA * Cross(rA, PA);
	cB += m_invMassB * PB;
	aB += m_invIB * Cross(rB, PB);

	data.positions[m_indexA].c = cA;
	data.positions[m_indexA].a = aA;
	data.positions[m_indexB].c = cB;
	data.positions[m_indexB].a = aB;

	return linearError < linearSlop;
}

Vec2 PulleyJoint::GetAnchorA() const
{
	return m_bodyA->GetWorldPoint(m_localAnchorA);
}

Vec2 PulleyJoint::GetAnchorB() const
{
	return m_bodyB->GetWorldPoint(m_localAnchorB);
}

Vec2 PulleyJoint::GetReactionForce(float32 inv_dt) const
{
	Vec2 P = m_impulse * m_uB;
	return inv_dt * P;
}

float32 PulleyJoint::GetReactionTorque(float32 inv_dt) const
{
	B2_NOT_USED(inv_dt);
	return 0.0f;
}

Vec2 PulleyJoint::GetGroundAnchorA() const
{
	return m_groundAnchorA;
}

Vec2 PulleyJoint::GetGroundAnchorB() const
{
	return m_groundAnchorB;
}

float32 PulleyJoint::GetLengthA() const
{
	return m_lengthA;
}

float32 PulleyJoint::GetLengthB() const
{
	return m_lengthB;
}

float32 PulleyJoint::GetRatio() const
{
	return m_ratio;
}

float32 PulleyJoint::GetCurrentLengthA() const
{
	Vec2 p = m_bodyA->GetWorldPoint(m_localAnchorA);
	Vec2 s = m_groundAnchorA;
	Vec2 d = p - s;
	return d.Length();
}

float32 PulleyJoint::GetCurrentLengthB() const
{
	Vec2 p = m_bodyB->GetWorldPoint(m_localAnchorB);
	Vec2 s = m_groundAnchorB;
	Vec2 d = p - s;
	return d.Length();
}

void PulleyJoint::Dump()
{
	int32 indexA = m_bodyA->m_islandIndex;
	int32 indexB = m_bodyB->m_islandIndex;

	Log("  b2::PulleyJointDef jd;\n");
	Log("  jd.bodyA = bodies[%d];\n", indexA);
	Log("  jd.bodyB = bodies[%d];\n", indexB);
	Log("  jd.collideConnected = bool(%d);\n", m_collideConnected);
	Log("  jd.groundAnchorA.Set(%.15lef, %.15lef);\n", m_groundAnchorA.x, m_groundAnchorA.y);
	Log("  jd.groundAnchorB.Set(%.15lef, %.15lef);\n", m_groundAnchorB.x, m_groundAnchorB.y);
	Log("  jd.localAnchorA.Set(%.15lef, %.15lef);\n", m_localAnchorA.x, m_localAnchorA.y);
	Log("  jd.localAnchorB.Set(%.15lef, %.15lef);\n", m_localAnchorB.x, m_localAnchorB.y);
	Log("  jd.lengthA = %.15lef;\n", m_lengthA);
	Log("  jd.lengthB = %.15lef;\n", m_lengthB);
	Log("  jd.ratio = %.15lef;\n", m_ratio);
	Log("  joints[%d] = m_world->CreateJoint(&jd);\n", m_index);
}

void PulleyJoint::ShiftOrigin(const Vec2& newOrigin)
{
	m_groundAnchorA -= newOrigin;
	m_groundAnchorB -= newOrigin;
}

} // namespace b2
