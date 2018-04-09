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

#include <Box2D/Dynamics/Contacts/ContactSolver.hpp>

#include <Box2D/Dynamics/Contacts/Contact.hpp>
#include <Box2D/Dynamics/Body.hpp>
#include <Box2D/Dynamics/Fixture.hpp>
#include <Box2D/Dynamics/World.hpp>
#include <Box2D/Common/StackAllocator.hpp>

#define B2_DEBUG_SOLVER 0

bool g_blockSolve = true;

namespace b2
{

struct ContactPositionConstraint
{
	Vec2 localPoints[maxManifoldPoints];
	Vec2 localNormal;
	Vec2 localPoint;
	int32 indexA;
	int32 indexB;
	float32 invMassA, invMassB;
	Vec2 localCenterA, localCenterB;
	float32 invIA, invIB;
	Manifold::Type type;
	float32 radiusA, radiusB;
	int32 pointCount;
};

ContactSolver::ContactSolver(ContactSolverDef* def)
{
	m_step = def->step;
	m_allocator = def->allocator;
	m_count = def->count;
	m_positionConstraints = (ContactPositionConstraint*)m_allocator->Allocate(m_count * sizeof(ContactPositionConstraint));
	m_velocityConstraints = (ContactVelocityConstraint*)m_allocator->Allocate(m_count * sizeof(ContactVelocityConstraint));
	m_positions = def->positions;
	m_velocities = def->velocities;
	m_contacts = def->contacts;

	// Initialize position independent portions of the constraints.
	for (int32 i = 0; i < m_count; ++i)
	{
		Contact* contact = m_contacts[i];

		Fixture* fixtureA = contact->m_fixtureA;
		Fixture* fixtureB = contact->m_fixtureB;
		Shape* shapeA = fixtureA->GetShape();
		Shape* shapeB = fixtureB->GetShape();
		float32 radiusA = shapeA->m_radius;
		float32 radiusB = shapeB->m_radius;
		Body* bodyA = fixtureA->GetBody();
		Body* bodyB = fixtureB->GetBody();
		Manifold* manifold = contact->GetManifold();

		int32 pointCount = manifold->pointCount;
		assert(pointCount > 0);

		ContactVelocityConstraint* vc = m_velocityConstraints + i;
		vc->friction = contact->m_friction;
		vc->restitution = contact->m_restitution;
		vc->tangentSpeed = contact->m_tangentSpeed;
		vc->indexA = bodyA->m_islandIndex;
		vc->indexB = bodyB->m_islandIndex;
		vc->invMassA = bodyA->m_invMass;
		vc->invMassB = bodyB->m_invMass;
		vc->invIA = bodyA->m_invI;
		vc->invIB = bodyB->m_invI;
		vc->contactIndex = i;
		vc->pointCount = pointCount;
		vc->K.SetZero();
		vc->normalMass.SetZero();

		ContactPositionConstraint* pc = m_positionConstraints + i;
		pc->indexA = bodyA->m_islandIndex;
		pc->indexB = bodyB->m_islandIndex;
		pc->invMassA = bodyA->m_invMass;
		pc->invMassB = bodyB->m_invMass;
		pc->localCenterA = bodyA->m_sweep.localCenter;
		pc->localCenterB = bodyB->m_sweep.localCenter;
		pc->invIA = bodyA->m_invI;
		pc->invIB = bodyB->m_invI;
		pc->localNormal = manifold->localNormal;
		pc->localPoint = manifold->localPoint;
		pc->pointCount = pointCount;
		pc->radiusA = radiusA;
		pc->radiusB = radiusB;
		pc->type = manifold->type;

		for (int32 j = 0; j < pointCount; ++j)
		{
			ManifoldPoint* cp = manifold->points + j;
			VelocityConstraintPoint* vcp = vc->points + j;
	
			if (m_step.warmStarting)
			{
				vcp->normalImpulse = m_step.dtRatio * cp->normalImpulse;
				vcp->tangentImpulse = m_step.dtRatio * cp->tangentImpulse;
			}
			else
			{
				vcp->normalImpulse = 0.0f;
				vcp->tangentImpulse = 0.0f;
			}

			vcp->rA.SetZero();
			vcp->rB.SetZero();
			vcp->normalMass = 0.0f;
			vcp->tangentMass = 0.0f;
			vcp->velocityBias = 0.0f;

			pc->localPoints[j] = cp->localPoint;
		}
	}
}

ContactSolver::~ContactSolver()
{
	m_allocator->Free(m_velocityConstraints);
	m_allocator->Free(m_positionConstraints);
}

// Initialize position dependent portions of the velocity constraints.
void ContactSolver::InitializeVelocityConstraints()
{
	for (int32 i = 0; i < m_count; ++i)
	{
		ContactVelocityConstraint* vc = m_velocityConstraints + i;
		ContactPositionConstraint* pc = m_positionConstraints + i;

		float32 radiusA = pc->radiusA;
		float32 radiusB = pc->radiusB;
		Manifold* manifold = m_contacts[vc->contactIndex]->GetManifold();

		int32 indexA = vc->indexA;
		int32 indexB = vc->indexB;

		float32 mA = vc->invMassA;
		float32 mB = vc->invMassB;
		float32 iA = vc->invIA;
		float32 iB = vc->invIB;
		Vec2 localCenterA = pc->localCenterA;
		Vec2 localCenterB = pc->localCenterB;

		Vec2 cA = m_positions[indexA].c;
		float32 aA = m_positions[indexA].a;
		Vec2 vA = m_velocities[indexA].v;
		float32 wA = m_velocities[indexA].w;

		Vec2 cB = m_positions[indexB].c;
		float32 aB = m_positions[indexB].a;
		Vec2 vB = m_velocities[indexB].v;
		float32 wB = m_velocities[indexB].w;

		assert(manifold->pointCount > 0);

		Transform xfA, xfB;
		xfA.q.Set(aA);
		xfB.q.Set(aB);
		xfA.p = cA - Mul(xfA.q, localCenterA);
		xfB.p = cB - Mul(xfB.q, localCenterB);

		WorldManifold worldManifold;
		worldManifold.Initialize(manifold, xfA, radiusA, xfB, radiusB);

		vc->normal = worldManifold.normal;

		int32 pointCount = vc->pointCount;
		for (int32 j = 0; j < pointCount; ++j)
		{
			VelocityConstraintPoint* vcp = vc->points + j;

			vcp->rA = worldManifold.points[j] - cA;
			vcp->rB = worldManifold.points[j] - cB;

			float32 rnA = Cross(vcp->rA, vc->normal);
			float32 rnB = Cross(vcp->rB, vc->normal);

			float32 kNormal = mA + mB + iA * rnA * rnA + iB * rnB * rnB;

			vcp->normalMass = kNormal > 0.0f ? 1.0f / kNormal : 0.0f;

			Vec2 tangent = Cross(vc->normal, 1.0f);

			float32 rtA = Cross(vcp->rA, tangent);
			float32 rtB = Cross(vcp->rB, tangent);

			float32 kTangent = mA + mB + iA * rtA * rtA + iB * rtB * rtB;

			vcp->tangentMass = kTangent > 0.0f ? 1.0f /  kTangent : 0.0f;

			// Setup a velocity bias for restitution.
			vcp->velocityBias = 0.0f;
			float32 vRel = Dot(vc->normal, vB + Cross(wB, vcp->rB) - vA - Cross(wA, vcp->rA));
			if (vRel < -velocityThreshold)
			{
				vcp->velocityBias = -vc->restitution * vRel;
			}
		}

		// If we have two points, then prepare the block solver.
		if (vc->pointCount == 2 && g_blockSolve)
		{
			VelocityConstraintPoint* vcp1 = vc->points + 0;
			VelocityConstraintPoint* vcp2 = vc->points + 1;

			float32 rn1A = Cross(vcp1->rA, vc->normal);
			float32 rn1B = Cross(vcp1->rB, vc->normal);
			float32 rn2A = Cross(vcp2->rA, vc->normal);
			float32 rn2B = Cross(vcp2->rB, vc->normal);

			float32 k11 = mA + mB + iA * rn1A * rn1A + iB * rn1B * rn1B;
			float32 k22 = mA + mB + iA * rn2A * rn2A + iB * rn2B * rn2B;
			float32 k12 = mA + mB + iA * rn1A * rn2A + iB * rn1B * rn2B;

			// Ensure a reasonable condition number.
			const float32 k_maxConditionNumber = 1000.0f;
			if (k11 * k11 < k_maxConditionNumber * (k11 * k22 - k12 * k12))
			{
				// K is safe to invert.
				vc->K.ex.Set(k11, k12);
				vc->K.ey.Set(k12, k22);
				vc->normalMass = vc->K.GetInverse();
			}
			else
			{
				// The constraints are redundant, just use one.
				// TODO_ERIN use deepest?
				vc->pointCount = 1;
			}
		}
	}
}

void ContactSolver::WarmStart()
{
	// Warm start.
	for (int32 i = 0; i < m_count; ++i)
	{
		ContactVelocityConstraint* vc = m_velocityConstraints + i;

		int32 indexA = vc->indexA;
		int32 indexB = vc->indexB;
		float32 mA = vc->invMassA;
		float32 iA = vc->invIA;
		float32 mB = vc->invMassB;
		float32 iB = vc->invIB;
		int32 pointCount = vc->pointCount;

		Vec2 vA = m_velocities[indexA].v;
		float32 wA = m_velocities[indexA].w;
		Vec2 vB = m_velocities[indexB].v;
		float32 wB = m_velocities[indexB].w;

		Vec2 normal = vc->normal;
		Vec2 tangent = Cross(normal, 1.0f);

		for (int32 j = 0; j < pointCount; ++j)
		{
			VelocityConstraintPoint* vcp = vc->points + j;
			Vec2 P = vcp->normalImpulse * normal + vcp->tangentImpulse * tangent;
			wA -= iA * Cross(vcp->rA, P);
			vA -= mA * P;
			wB += iB * Cross(vcp->rB, P);
			vB += mB * P;
		}

		m_velocities[indexA].v = vA;
		m_velocities[indexA].w = wA;
		m_velocities[indexB].v = vB;
		m_velocities[indexB].w = wB;
	}
}

void ContactSolver::SolveVelocityConstraints()
{
	for (int32 i = 0; i < m_count; ++i)
	{
		ContactVelocityConstraint* vc = m_velocityConstraints + i;

		int32 indexA = vc->indexA;
		int32 indexB = vc->indexB;
		float32 mA = vc->invMassA;
		float32 iA = vc->invIA;
		float32 mB = vc->invMassB;
		float32 iB = vc->invIB;
		int32 pointCount = vc->pointCount;

		Vec2 vA = m_velocities[indexA].v;
		float32 wA = m_velocities[indexA].w;
		Vec2 vB = m_velocities[indexB].v;
		float32 wB = m_velocities[indexB].w;

		Vec2 normal = vc->normal;
		Vec2 tangent = Cross(normal, 1.0f);
		float32 friction = vc->friction;

		assert(pointCount == 1 || pointCount == 2);

		// Solve tangent constraints first because non-penetration is more important
		// than friction.
		for (int32 j = 0; j < pointCount; ++j)
		{
			VelocityConstraintPoint* vcp = vc->points + j;

			// Relative velocity at contact
			Vec2 dv = vB + Cross(wB, vcp->rB) - vA - Cross(wA, vcp->rA);

			// Compute tangent force
			float32 vt = Dot(dv, tangent) - vc->tangentSpeed;
			float32 lambda = vcp->tangentMass * (-vt);

			// b2::Clamp the accumulated force
			float32 maxFriction = friction * vcp->normalImpulse;
			float32 newImpulse = Clamp(vcp->tangentImpulse + lambda, -maxFriction, maxFriction);
			lambda = newImpulse - vcp->tangentImpulse;
			vcp->tangentImpulse = newImpulse;

			// Apply contact impulse
			Vec2 P = lambda * tangent;

			vA -= mA * P;
			wA -= iA * Cross(vcp->rA, P);

			vB += mB * P;
			wB += iB * Cross(vcp->rB, P);
		}

		// Solve normal constraints
		if (pointCount == 1 || g_blockSolve == false)
		{
			for (int32 i = 0; i < pointCount; ++i)
			{
				VelocityConstraintPoint* vcp = vc->points + i;

				// Relative velocity at contact
				Vec2 dv = vB + Cross(wB, vcp->rB) - vA - Cross(wA, vcp->rA);

				// Compute normal impulse
				float32 vn = Dot(dv, normal);
				float32 lambda = -vcp->normalMass * (vn - vcp->velocityBias);

				// b2::Clamp the accumulated impulse
				float32 newImpulse = Max(vcp->normalImpulse + lambda, 0.0f);
				lambda = newImpulse - vcp->normalImpulse;
				vcp->normalImpulse = newImpulse;

				// Apply contact impulse
				Vec2 P = lambda * normal;
				vA -= mA * P;
				wA -= iA * Cross(vcp->rA, P);

				vB += mB * P;
				wB += iB * Cross(vcp->rB, P);
			}
		}
		else
		{
			// Block solver developed in collaboration with Dirk Gregorius (back in 01/07 on Box2D_Lite).
			// Build the mini LCP for this contact patch
			//
			// vn = A * x + b, vn >= 0, , vn >= 0, x >= 0 and vn_i * x_i = 0 with i = 1..2
			//
			// A = J * W * JT and J = ( -n, -r1 x n, n, r2 x n )
			// b = vn0 - velocityBias
			//
			// The system is solved using the "Total enumeration method" (s. Murty). The complementary constraint vn_i * x_i
			// implies that we must have in any solution either vn_i = 0 or x_i = 0. So for the 2D contact problem the cases
			// vn1 = 0 and vn2 = 0, x1 = 0 and x2 = 0, x1 = 0 and vn2 = 0, x2 = 0 and vn1 = 0 need to be tested. The first valid
			// solution that satisfies the problem is chosen.
			// 
			// In order to account of the accumulated impulse 'a' (because of the iterative nature of the solver which only requires
			// that the accumulated impulse is clamped and not the incremental impulse) we change the impulse variable (x_i).
			//
			// Substitute:
			// 
			// x = a + d
			// 
			// a := old total impulse
			// x := new total impulse
			// d := incremental impulse 
			//
			// For the current iteration we extend the formula for the incremental impulse
			// to compute the new total impulse:
			//
			// vn = A * d + b
			//    = A * (x - a) + b
			//    = A * x + b - A * a
			//    = A * x + b'
			// b' = b - A * a;

			VelocityConstraintPoint* cp1 = vc->points + 0;
			VelocityConstraintPoint* cp2 = vc->points + 1;

			Vec2 a(cp1->normalImpulse, cp2->normalImpulse);
			assert(a.x >= 0.0f && a.y >= 0.0f);

			// Relative velocity at contact
			Vec2 dv1 = vB + Cross(wB, cp1->rB) - vA - Cross(wA, cp1->rA);
			Vec2 dv2 = vB + Cross(wB, cp2->rB) - vA - Cross(wA, cp2->rA);

			// Compute normal velocity
			float32 vn1 = Dot(dv1, normal);
			float32 vn2 = Dot(dv2, normal);

			Vec2 b;
			b.x = vn1 - cp1->velocityBias;
			b.y = vn2 - cp2->velocityBias;

			// Compute b'
			b -= Mul(vc->K, a);

			const float32 k_errorTol = 1e-3f;
			B2_NOT_USED(k_errorTol);

			for (;;)
			{
				//
				// Case 1: vn = 0
				//
				// 0 = A * x + b'
				//
				// Solve for x:
				//
				// x = - inv(A) * b'
				//
				Vec2 x = - Mul(vc->normalMass, b);

				if (x.x >= 0.0f && x.y >= 0.0f)
				{
					// Get the incremental impulse
					Vec2 d = x - a;

					// Apply incremental impulse
					Vec2 P1 = d.x * normal;
					Vec2 P2 = d.y * normal;
					vA -= mA * (P1 + P2);
					wA -= iA * (Cross(cp1->rA, P1) + Cross(cp2->rA, P2));

					vB += mB * (P1 + P2);
					wB += iB * (Cross(cp1->rB, P1) + Cross(cp2->rB, P2));

					// Accumulate
					cp1->normalImpulse = x.x;
					cp2->normalImpulse = x.y;

#if B2_DEBUG_SOLVER == 1
					// Postconditions
					dv1 = vB + Cross(wB, cp1->rB) - vA - Cross(wA, cp1->rA);
					dv2 = vB + Cross(wB, cp2->rB) - vA - Cross(wA, cp2->rA);

					// Compute normal velocity
					vn1 = Dot(dv1, normal);
					vn2 = Dot(dv2, normal);

					assert(Abs(vn1 - cp1->velocityBias) < k_errorTol);
					assert(Abs(vn2 - cp2->velocityBias) < k_errorTol);
#endif
					break;
				}

				//
				// Case 2: vn1 = 0 and x2 = 0
				//
				//   0 = a11 * x1 + a12 * 0 + b1' 
				// vn2 = a21 * x1 + a22 * 0 + b2'
				//
				x.x = - cp1->normalMass * b.x;
				x.y = 0.0f;
				vn1 = 0.0f;
				vn2 = vc->K.ex.y * x.x + b.y;

				if (x.x >= 0.0f && vn2 >= 0.0f)
				{
					// Get the incremental impulse
					Vec2 d = x - a;

					// Apply incremental impulse
					Vec2 P1 = d.x * normal;
					Vec2 P2 = d.y * normal;
					vA -= mA * (P1 + P2);
					wA -= iA * (Cross(cp1->rA, P1) + Cross(cp2->rA, P2));

					vB += mB * (P1 + P2);
					wB += iB * (Cross(cp1->rB, P1) + Cross(cp2->rB, P2));

					// Accumulate
					cp1->normalImpulse = x.x;
					cp2->normalImpulse = x.y;

#if B2_DEBUG_SOLVER == 1
					// Postconditions
					dv1 = vB + Cross(wB, cp1->rB) - vA - Cross(wA, cp1->rA);

					// Compute normal velocity
					vn1 = Dot(dv1, normal);

					assert(Abs(vn1 - cp1->velocityBias) < k_errorTol);
#endif
					break;
				}


				//
				// Case 3: vn2 = 0 and x1 = 0
				//
				// vn1 = a11 * 0 + a12 * x2 + b1' 
				//   0 = a21 * 0 + a22 * x2 + b2'
				//
				x.x = 0.0f;
				x.y = - cp2->normalMass * b.y;
				vn1 = vc->K.ey.x * x.y + b.x;
				vn2 = 0.0f;

				if (x.y >= 0.0f && vn1 >= 0.0f)
				{
					// Resubstitute for the incremental impulse
					Vec2 d = x - a;

					// Apply incremental impulse
					Vec2 P1 = d.x * normal;
					Vec2 P2 = d.y * normal;
					vA -= mA * (P1 + P2);
					wA -= iA * (Cross(cp1->rA, P1) + Cross(cp2->rA, P2));

					vB += mB * (P1 + P2);
					wB += iB * (Cross(cp1->rB, P1) + Cross(cp2->rB, P2));

					// Accumulate
					cp1->normalImpulse = x.x;
					cp2->normalImpulse = x.y;

#if B2_DEBUG_SOLVER == 1
					// Postconditions
					dv2 = vB + Cross(wB, cp2->rB) - vA - Cross(wA, cp2->rA);

					// Compute normal velocity
					vn2 = Dot(dv2, normal);

					assert(Abs(vn2 - cp2->velocityBias) < k_errorTol);
#endif
					break;
				}

				//
				// Case 4: x1 = 0 and x2 = 0
				// 
				// vn1 = b1
				// vn2 = b2;
				x.x = 0.0f;
				x.y = 0.0f;
				vn1 = b.x;
				vn2 = b.y;

				if (vn1 >= 0.0f && vn2 >= 0.0f )
				{
					// Resubstitute for the incremental impulse
					Vec2 d = x - a;

					// Apply incremental impulse
					Vec2 P1 = d.x * normal;
					Vec2 P2 = d.y * normal;
					vA -= mA * (P1 + P2);
					wA -= iA * (Cross(cp1->rA, P1) + Cross(cp2->rA, P2));

					vB += mB * (P1 + P2);
					wB += iB * (Cross(cp1->rB, P1) + Cross(cp2->rB, P2));

					// Accumulate
					cp1->normalImpulse = x.x;
					cp2->normalImpulse = x.y;

					break;
				}

				// No solution, give up. This is hit sometimes, but it doesn't seem to matter.
				break;
			}
		}

		m_velocities[indexA].v = vA;
		m_velocities[indexA].w = wA;
		m_velocities[indexB].v = vB;
		m_velocities[indexB].w = wB;
	}
}

void ContactSolver::StoreImpulses()
{
	for (int32 i = 0; i < m_count; ++i)
	{
		ContactVelocityConstraint* vc = m_velocityConstraints + i;
		Manifold* manifold = m_contacts[vc->contactIndex]->GetManifold();

		for (int32 j = 0; j < vc->pointCount; ++j)
		{
			manifold->points[j].normalImpulse = vc->points[j].normalImpulse;
			manifold->points[j].tangentImpulse = vc->points[j].tangentImpulse;
		}
	}
}

struct PositionSolverManifold
{
	void Initialize(ContactPositionConstraint* pc, const Transform& xfA, const Transform& xfB, int32 index)
	{
		assert(pc->pointCount > 0);

		switch (pc->type)
		{
		case Manifold::e_circles:
			{
				Vec2 pointA = Mul(xfA, pc->localPoint);
				Vec2 pointB = Mul(xfB, pc->localPoints[0]);
				normal = pointB - pointA;
				normal.Normalize();
				point = 0.5f * (pointA + pointB);
				separation = Dot(pointB - pointA, normal) - pc->radiusA - pc->radiusB;
			}
			break;

		case Manifold::e_faceA:
			{
				normal = Mul(xfA.q, pc->localNormal);
				Vec2 planePoint = Mul(xfA, pc->localPoint);

				Vec2 clipPoint = Mul(xfB, pc->localPoints[index]);
				separation = Dot(clipPoint - planePoint, normal) - pc->radiusA - pc->radiusB;
				point = clipPoint;
			}
			break;

		case Manifold::e_faceB:
			{
				normal = Mul(xfB.q, pc->localNormal);
				Vec2 planePoint = Mul(xfB, pc->localPoint);

				Vec2 clipPoint = Mul(xfA, pc->localPoints[index]);
				separation = Dot(clipPoint - planePoint, normal) - pc->radiusA - pc->radiusB;
				point = clipPoint;

				// Ensure normal points from A to B
				normal = -normal;
			}
			break;
		}
	}

	Vec2 normal;
	Vec2 point;
	float32 separation;
};

// Sequential solver.
bool ContactSolver::SolvePositionConstraints()
{
	float32 minSeparation = 0.0f;

	for (int32 i = 0; i < m_count; ++i)
	{
		ContactPositionConstraint* pc = m_positionConstraints + i;

		int32 indexA = pc->indexA;
		int32 indexB = pc->indexB;
		Vec2 localCenterA = pc->localCenterA;
		float32 mA = pc->invMassA;
		float32 iA = pc->invIA;
		Vec2 localCenterB = pc->localCenterB;
		float32 mB = pc->invMassB;
		float32 iB = pc->invIB;
		int32 pointCount = pc->pointCount;

		Vec2 cA = m_positions[indexA].c;
		float32 aA = m_positions[indexA].a;

		Vec2 cB = m_positions[indexB].c;
		float32 aB = m_positions[indexB].a;

		// Solve normal constraints
		for (int32 j = 0; j < pointCount; ++j)
		{
			Transform xfA, xfB;
			xfA.q.Set(aA);
			xfB.q.Set(aB);
			xfA.p = cA - Mul(xfA.q, localCenterA);
			xfB.p = cB - Mul(xfB.q, localCenterB);

			PositionSolverManifold psm;
			psm.Initialize(pc, xfA, xfB, j);
			Vec2 normal = psm.normal;

			Vec2 point = psm.point;
			float32 separation = psm.separation;

			Vec2 rA = point - cA;
			Vec2 rB = point - cB;

			// Track max constraint error.
			minSeparation = Min(minSeparation, separation);

			// Prevent large corrections and allow slop.
			float32 C = Clamp(baumgarte * (separation + linearSlop), -maxLinearCorrection, 0.0f);

			// Compute the effective mass.
			float32 rnA = Cross(rA, normal);
			float32 rnB = Cross(rB, normal);
			float32 K = mA + mB + iA * rnA * rnA + iB * rnB * rnB;

			// Compute normal impulse
			float32 impulse = K > 0.0f ? - C / K : 0.0f;

			Vec2 P = impulse * normal;

			cA -= mA * P;
			aA -= iA * Cross(rA, P);

			cB += mB * P;
			aB += iB * Cross(rB, P);
		}

		m_positions[indexA].c = cA;
		m_positions[indexA].a = aA;

		m_positions[indexB].c = cB;
		m_positions[indexB].a = aB;
	}

	// We can't expect minSpeparation >= -linearSlop because we don't
	// push the separation above -linearSlop.
	return minSeparation >= -3.0f * linearSlop;
}

// Sequential position solver for position constraints.
bool ContactSolver::SolveTOIPositionConstraints(int32 toiIndexA, int32 toiIndexB)
{
	float32 minSeparation = 0.0f;

	for (int32 i = 0; i < m_count; ++i)
	{
		ContactPositionConstraint* pc = m_positionConstraints + i;

		int32 indexA = pc->indexA;
		int32 indexB = pc->indexB;
		Vec2 localCenterA = pc->localCenterA;
		Vec2 localCenterB = pc->localCenterB;
		int32 pointCount = pc->pointCount;

		float32 mA = 0.0f;
		float32 iA = 0.0f;
		if (indexA == toiIndexA || indexA == toiIndexB)
		{
			mA = pc->invMassA;
			iA = pc->invIA;
		}

		float32 mB = 0.0f;
		float32 iB = 0.;
		if (indexB == toiIndexA || indexB == toiIndexB)
		{
			mB = pc->invMassB;
			iB = pc->invIB;
		}

		Vec2 cA = m_positions[indexA].c;
		float32 aA = m_positions[indexA].a;

		Vec2 cB = m_positions[indexB].c;
		float32 aB = m_positions[indexB].a;

		// Solve normal constraints
		for (int32 j = 0; j < pointCount; ++j)
		{
			Transform xfA, xfB;
			xfA.q.Set(aA);
			xfB.q.Set(aB);
			xfA.p = cA - Mul(xfA.q, localCenterA);
			xfB.p = cB - Mul(xfB.q, localCenterB);

			PositionSolverManifold psm;
			psm.Initialize(pc, xfA, xfB, j);
			Vec2 normal = psm.normal;

			Vec2 point = psm.point;
			float32 separation = psm.separation;

			Vec2 rA = point - cA;
			Vec2 rB = point - cB;

			// Track max constraint error.
			minSeparation = Min(minSeparation, separation);

			// Prevent large corrections and allow slop.
			float32 C = Clamp(toiBaugarte * (separation + linearSlop), -maxLinearCorrection, 0.0f);

			// Compute the effective mass.
			float32 rnA = Cross(rA, normal);
			float32 rnB = Cross(rB, normal);
			float32 K = mA + mB + iA * rnA * rnA + iB * rnB * rnB;

			// Compute normal impulse
			float32 impulse = K > 0.0f ? - C / K : 0.0f;

			Vec2 P = impulse * normal;

			cA -= mA * P;
			aA -= iA * Cross(rA, P);

			cB += mB * P;
			aB += iB * Cross(rB, P);
		}

		m_positions[indexA].c = cA;
		m_positions[indexA].a = aA;

		m_positions[indexB].c = cB;
		m_positions[indexB].a = aB;
	}

	// We can't expect minSpeparation >= -linearSlop because we don't
	// push the separation above -linearSlop.
	return minSeparation >= -1.5f * linearSlop;
}

} // namespace b2
