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

#include <Box2D/Dynamics/Joints/DistanceJoint.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/TimeStep.hpp>

namespace b2
{

// 1-D constrained system
// m (v2 - v1) = lambda
// v2 + (beta/h) * x1 + gamma * lambda = 0, gamma has units of inverse mass.
// x2 = x1 + h * v2

// 1-D mass-damper-spring system
// m (v2 - v1) + h * d * v2 + h * k * 

// C = norm(p2 - p1) - L
// u = (p2 - p1) / norm(p2 - p1)
// Cdot = dot(u, v2 + cross(w2, r2) - v1 - cross(w1, r1))
// J = [-u -cross(r1, u) u cross(r2, u)]
// K = J * invM * JT
//   = invMass1 + invI1 * cross(r1, u)^2 + invMass2 + invI2 * cross(r2, u)^2

void DistanceJointDef::Initialize(Body* b1, Body* b2,
								  const Vec2& anchor1, const Vec2& anchor2)
{
	bodyA = b1;
	bodyB = b2;
	localAnchorA = bodyA->GetLocalPoint(anchor1);
	localAnchorB = bodyB->GetLocalPoint(anchor2);
	Vec2 d = anchor2 - anchor1;
	length = d.Length();
}

DistanceJoint::DistanceJoint(const DistanceJointDef* def)
: Joint(def)
{
	m_localAnchorA = def->localAnchorA;
	m_localAnchorB = def->localAnchorB;
	m_length = def->length;
	m_frequencyHz = def->frequencyHz;
	m_dampingRatio = def->dampingRatio;
	m_impulse = 0.0f;
	m_gamma = 0.0f;
	m_bias = 0.0f;
}

void DistanceJoint::InitVelocityConstraints(const SolverData& data)
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

	// Handle singularity.
	float32 length = m_u.Length();
	if (length > linearSlop)
	{
		m_u *= 1.0f / length;
	}
	else
	{
		m_u.Set(0.0f, 0.0f);
	}

	float32 crAu = Cross(m_rA, m_u);
	float32 crBu = Cross(m_rB, m_u);
	float32 invMass = m_invMassA + m_invIA * crAu * crAu + m_invMassB + m_invIB * crBu * crBu;

	// Compute the effective mass matrix.
	m_mass = invMass != 0.0f ? 1.0f / invMass : 0.0f;

	if (m_frequencyHz > 0.0f)
	{
		float32 C = length - m_length;

		// Frequency
		float32 omega = 2.0f * pi * m_frequencyHz;

		// Damping coefficient
		float32 d = 2.0f * m_mass * m_dampingRatio * omega;

		// Spring stiffness
		float32 k = m_mass * omega * omega;

		// magic formulas
		float32 h = data.step.dt;
		m_gamma = h * (d + h * k);
		m_gamma = m_gamma != 0.0f ? 1.0f / m_gamma : 0.0f;
		m_bias = C * h * k * m_gamma;

		invMass += m_gamma;
		m_mass = invMass != 0.0f ? 1.0f / invMass : 0.0f;
	}
	else
	{
		m_gamma = 0.0f;
		m_bias = 0.0f;
	}

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

void DistanceJoint::SolveVelocityConstraints(const SolverData& data)
{
	Vec2 vA = data.velocities[m_indexA].v;
	float32 wA = data.velocities[m_indexA].w;
	Vec2 vB = data.velocities[m_indexB].v;
	float32 wB = data.velocities[m_indexB].w;

	// Cdot = dot(u, v + cross(w, r))
	Vec2 vpA = vA + Cross(wA, m_rA);
	Vec2 vpB = vB + Cross(wB, m_rB);
	float32 Cdot = Dot(m_u, vpB - vpA);

	float32 impulse = -m_mass * (Cdot + m_bias + m_gamma * m_impulse);
	m_impulse += impulse;

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

bool DistanceJoint::SolvePositionConstraints(const SolverData& data)
{
	if (m_frequencyHz > 0.0f)
	{
		// There is no position correction for soft distance constraints.
		return true;
	}

	Vec2 cA = data.positions[m_indexA].c;
	float32 aA = data.positions[m_indexA].a;
	Vec2 cB = data.positions[m_indexB].c;
	float32 aB = data.positions[m_indexB].a;

	Rot qA(aA), qB(aB);

	Vec2 rA = Mul(qA, m_localAnchorA - m_localCenterA);
	Vec2 rB = Mul(qB, m_localAnchorB - m_localCenterB);
	Vec2 u = cB + rB - cA - rA;

	float32 length = u.Normalize();
	float32 C = length - m_length;
	C = Clamp(C, -maxLinearCorrection, maxLinearCorrection);

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

	return Abs(C) < linearSlop;
}

Vec2 DistanceJoint::GetAnchorA() const
{
	return m_bodyA->GetWorldPoint(m_localAnchorA);
}

Vec2 DistanceJoint::GetAnchorB() const
{
	return m_bodyB->GetWorldPoint(m_localAnchorB);
}

Vec2 DistanceJoint::GetReactionForce(float32 inv_dt) const
{
	Vec2 F = (inv_dt * m_impulse) * m_u;
	return F;
}

float32 DistanceJoint::GetReactionTorque(float32 inv_dt) const
{
	B2_NOT_USED(inv_dt);
	return 0.0f;
}

void DistanceJoint::Dump()
{
	int32 indexA = m_bodyA->m_islandIndex;
	int32 indexB = m_bodyB->m_islandIndex;

	Log("  b2::DistanceJointDef jd;\n");
	Log("  jd.bodyA = bodies[%d];\n", indexA);
	Log("  jd.bodyB = bodies[%d];\n", indexB);
	Log("  jd.collideConnected = bool(%d);\n", m_collideConnected);
	Log("  jd.localAnchorA.Set(%.15lef, %.15lef);\n", m_localAnchorA.x, m_localAnchorA.y);
	Log("  jd.localAnchorB.Set(%.15lef, %.15lef);\n", m_localAnchorB.x, m_localAnchorB.y);
	Log("  jd.length = %.15lef;\n", m_length);
	Log("  jd.frequencyHz = %.15lef;\n", m_frequencyHz);
	Log("  jd.dampingRatio = %.15lef;\n", m_dampingRatio);
	Log("  joints[%d] = m_world->CreateJoint(&jd);\n", m_index);
}

} // namespace b2
