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

#include <Box2D/Dynamics/Joints/MouseJoint.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/TimeStep.hpp>

namespace b2
{

// p = attached point, m = mouse point
// C = p - m
// Cdot = v
//      = v + cross(w, r)
// J = [I r_skew]
// Identity used:
// w k % (rx i + ry j) = w * (-ry i + rx j)

MouseJoint::MouseJoint(const MouseJointDef* def)
: Joint(def)
{
	assert(def->target.IsValid());
	assert(IsValid(def->maxForce) && def->maxForce >= 0.0f);
	assert(IsValid(def->frequencyHz) && def->frequencyHz >= 0.0f);
	assert(IsValid(def->dampingRatio) && def->dampingRatio >= 0.0f);

	m_targetA = def->target;
	m_localAnchorB = MulT(m_bodyB->GetTransform(), m_targetA);

	m_maxForce = def->maxForce;
	m_impulse.SetZero();

	m_frequencyHz = def->frequencyHz;
	m_dampingRatio = def->dampingRatio;

	m_beta = 0.0f;
	m_gamma = 0.0f;
}

void MouseJoint::SetTarget(const Vec2& target)
{
	if (m_bodyB->IsAwake() == false)
	{
		m_bodyB->SetAwake(true);
	}
	m_targetA = target;
}

const Vec2& MouseJoint::GetTarget() const
{
	return m_targetA;
}

void MouseJoint::SetMaxForce(float32 force)
{
	m_maxForce = force;
}

float32 MouseJoint::GetMaxForce() const
{
	return m_maxForce;
}

void MouseJoint::SetFrequency(float32 hz)
{
	m_frequencyHz = hz;
}

float32 MouseJoint::GetFrequency() const
{
	return m_frequencyHz;
}

void MouseJoint::SetDampingRatio(float32 ratio)
{
	m_dampingRatio = ratio;
}

float32 MouseJoint::GetDampingRatio() const
{
	return m_dampingRatio;
}

void MouseJoint::InitVelocityConstraints(const SolverData& data)
{
	m_indexB = m_bodyB->m_islandIndex;
	m_localCenterB = m_bodyB->m_sweep.localCenter;
	m_invMassB = m_bodyB->m_invMass;
	m_invIB = m_bodyB->m_invI;

	Vec2 cB = data.positions[m_indexB].c;
	float32 aB = data.positions[m_indexB].a;
	Vec2 vB = data.velocities[m_indexB].v;
	float32 wB = data.velocities[m_indexB].w;

	Rot qB(aB);

	float32 mass = m_bodyB->GetMass();

	// Frequency
	float32 omega = 2.0f * pi * m_frequencyHz;

	// Damping coefficient
	float32 d = 2.0f * mass * m_dampingRatio * omega;

	// Spring stiffness
	float32 k = mass * (omega * omega);

	// magic formulas
	// gamma has units of inverse mass.
	// beta has units of inverse time.
	float32 h = data.step.dt;
	assert(d + h * k > epsilon);
	m_gamma = h * (d + h * k);
	if (m_gamma != 0.0f)
	{
		m_gamma = 1.0f / m_gamma;
	}
	m_beta = h * k * m_gamma;

	// Compute the effective mass matrix.
	m_rB = Mul(qB, m_localAnchorB - m_localCenterB);

	// K    = [(1/m1 + 1/m2) * eye(2) - skew(r1) * invI1 * skew(r1) - skew(r2) * invI2 * skew(r2)]
	//      = [1/m1+1/m2     0    ] + invI1 * [r1.y*r1.y -r1.x*r1.y] + invI2 * [r1.y*r1.y -r1.x*r1.y]
	//        [    0     1/m1+1/m2]           [-r1.x*r1.y r1.x*r1.x]           [-r1.x*r1.y r1.x*r1.x]
	Mat22 K;
	K.ex.x = m_invMassB + m_invIB * m_rB.y * m_rB.y + m_gamma;
	K.ex.y = -m_invIB * m_rB.x * m_rB.y;
	K.ey.x = K.ex.y;
	K.ey.y = m_invMassB + m_invIB * m_rB.x * m_rB.x + m_gamma;

	m_mass = K.GetInverse();

	m_C = cB + m_rB - m_targetA;
	m_C *= m_beta;

	// Cheat with some damping
	wB *= 0.98f;

	if (data.step.warmStarting)
	{
		m_impulse *= data.step.dtRatio;
		vB += m_invMassB * m_impulse;
		wB += m_invIB * Cross(m_rB, m_impulse);
	}
	else
	{
		m_impulse.SetZero();
	}

	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

void MouseJoint::SolveVelocityConstraints(const SolverData& data)
{
	Vec2 vB = data.velocities[m_indexB].v;
	float32 wB = data.velocities[m_indexB].w;

	// Cdot = v + cross(w, r)
	Vec2 Cdot = vB + Cross(wB, m_rB);
	Vec2 impulse = Mul(m_mass, -(Cdot + m_C + m_gamma * m_impulse));

	Vec2 oldImpulse = m_impulse;
	m_impulse += impulse;
	float32 maxImpulse = data.step.dt * m_maxForce;
	if (m_impulse.LengthSquared() > maxImpulse * maxImpulse)
	{
		m_impulse *= maxImpulse / m_impulse.Length();
	}
	impulse = m_impulse - oldImpulse;

	vB += m_invMassB * impulse;
	wB += m_invIB * Cross(m_rB, impulse);

	data.velocities[m_indexB].v = vB;
	data.velocities[m_indexB].w = wB;
}

bool MouseJoint::SolvePositionConstraints(const SolverData& data)
{
	B2_NOT_USED(data);
	return true;
}

Vec2 MouseJoint::GetAnchorA() const
{
	return m_targetA;
}

Vec2 MouseJoint::GetAnchorB() const
{
	return m_bodyB->GetWorldPoint(m_localAnchorB);
}

Vec2 MouseJoint::GetReactionForce(float32 inv_dt) const
{
	return inv_dt * m_impulse;
}

float32 MouseJoint::GetReactionTorque(float32 inv_dt) const
{
	return inv_dt * 0.0f;
}

void MouseJoint::ShiftOrigin(const Vec2& newOrigin)
{
	m_targetA -= newOrigin;
}

} // namespace b2
