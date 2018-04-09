/*
* Copyright (c) 2007-2011 Erin Catto http://www.box2d.org
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

#include <Box2D/Dynamics/Joints/RopeJoint.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/TimeStep.hpp>

namespace b2
{

// Limit:
// C = norm(pB - pA) - L
// u = (pB - pA) / norm(pB - pA)
// Cdot = dot(u, vB + cross(wB, rB) - vA - cross(wA, rA))
// J = [-u -cross(rA, u) u cross(rB, u)]
// K = J * invM * JT
//   = invMassA + invIA * cross(rA, u)^2 + invMassB + invIB * cross(rB, u)^2

RopeJoint::RopeJoint(const RopeJointDef* def)
: Joint(def)
{
	m_localAnchorA = def->localAnchorA;
	m_localAnchorB = def->localAnchorB;

	m_maxLength = def->maxLength;

	m_mass = 0.0f;
	m_impulse = 0.0f;
	m_state = e_inactiveLimit;
	m_length = 0.0f;
}

void RopeJoint::InitVelocityConstraints(const SolverData& data)
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
	m_u = cB + m_rB - cA - m_rA;

	m_length = m_u.Length();

	float32 C = m_length - m_maxLength;
	if (C > 0.0f)
	{
		m_state = e_atUpperLimit;
	}
	else
	{
		m_state = e_inactiveLimit;
	}

	if (m_length > linearSlop)
	{
		m_u *= 1.0f / m_length;
	}
	else
	{
		m_u.SetZero();
		m_mass = 0.0f;
		m_impulse = 0.0f;
		return;
	}

	// Compute effective mass.
	float32 crA = Cross(m_rA, m_u);
	float32 crB = Cross(m_rB, m_u);
	float32 invMass = m_invMassA + m_invIA * crA * crA + m_invMassB + m_invIB * crB * crB;

	m_mass = invMass != 0.0f ? 1.0f / invMass : 0.0f;

	if (data.step.warmStarting)
	{
		// Scale the impulse to support a variable time step.
		m_impulse *= data.step.dtRatio;

		Vec2 P = m_impulse * m_u;
		vA -= m_invMassA * P;
		wA -= m_invIA * Cross(m_rA, P);
		vB += m_invMassB * P;
		wB += m_invIB * Cross(m_rB, P);
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

void RopeJoint::SolveVelocityConstraints(const SolverData& data)
{
	Vec2 vA = data.velocities[m_indexA].v;
	float32 wA = data.velocities[m_indexA].w;
	Vec2 vB = data.velocities[m_indexB].v;
	float32 wB = data.velocities[m_indexB].w;

	// Cdot = dot(u, v + cross(w, r))
	Vec2 vpA = vA + Cross(wA, m_rA);
	Vec2 vpB = vB + Cross(wB, m_rB);
	float32 C = m_length - m_maxLength;
	float32 Cdot = Dot(m_u, vpB - vpA);

	// Predictive constraint.
	if (C < 0.0f)
	{
		Cdot += data.step.inv_dt * C;
	}

	float32 impulse = -m_mass * Cdot;
	float32 oldImpulse = m_impulse;
	m_impulse = Min(0.0f, m_impulse + impulse);
	impulse = m_impulse - oldImpulse;

	Vec2 P = impulse * m_u;
	vA -= m_invMassA * P;
	wA -= m_invIA * Cross(m_rA, P);
	vB += m_invMassB * P;
	wB += m_invIB * Cross(m_rB, P);

	data.velocities[m_indexA].v = vA;
	data.velocities[m_indexA].w = wA;
	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

bool RopeJoint::SolvePositionConstraints(const SolverData& data)
{
	Vec2 cA = data.positions[m_indexA].c;
	float32 aA = data.positions[m_indexA].a;
	Vec2 cB = data.positions[m_indexB].c;
	float32 aB = data.positions[m_indexB].a;

	Rot qA(aA), qB(aB);

	Vec2 rA = Mul(qA, m_localAnchorA - m_localCenterA);
	Vec2 rB = Mul(qB, m_localAnchorB - m_localCenterB);
	Vec2 u = cB + rB - cA - rA;

	float32 length = u.Normalize();
	float32 C = length - m_maxLength;

	C = Clamp(C, 0.0f, maxLinearCorrection);

	float32 impulse = -m_mass * C;
	Vec2 P = impulse * u;

	cA -= m_invMassA * P;
	aA -= m_invIA * Cross(rA, P);
	cB += m_invMassB * P;
	aB += m_invIB * Cross(rB, P);

	data.positions[m_indexA].c = cA;
	data.positions[m_indexA].a = aA;
	data.positions[m_indexB].c = cB;
	data.positions[m_indexB].a = aB;

	return length - m_maxLength < linearSlop;
}

Vec2 RopeJoint::GetAnchorA() const
{
	return m_bodyA->GetWorldPoint(m_localAnchorA);
}

Vec2 RopeJoint::GetAnchorB() const
{
	return m_bodyB->GetWorldPoint(m_localAnchorB);
}

Vec2 RopeJoint::GetReactionForce(float32 inv_dt) const
{
	Vec2 F = (inv_dt * m_impulse) * m_u;
	return F;
}

float32 RopeJoint::GetReactionTorque(float32 inv_dt) const
{
	B2_NOT_USED(inv_dt);
	return 0.0f;
}

float32 RopeJoint::GetMaxLength() const
{
	return m_maxLength;
}

LimitState RopeJoint::GetLimitState() const
{
	return m_state;
}

void RopeJoint::Dump()
{
	int32 indexA = m_bodyA->m_islandIndex;
	int32 indexB = m_bodyB->m_islandIndex;

	Log("  b2::RopeJointDef jd;\n");
	Log("  jd.bodyA = bodies[%d];\n", indexA);
	Log("  jd.bodyB = bodies[%d];\n", indexB);
	Log("  jd.collideConnected = bool(%d);\n", m_collideConnected);
	Log("  jd.localAnchorA.Set(%.15lef, %.15lef);\n", m_localAnchorA.x, m_localAnchorA.y);
	Log("  jd.localAnchorB.Set(%.15lef, %.15lef);\n", m_localAnchorB.x, m_localAnchorB.y);
	Log("  jd.maxLength = %.15lef;\n", m_maxLength);
	Log("  joints[%d] = m_world->CreateJoint(&jd);\n", m_index);
}

} // namespace b2
