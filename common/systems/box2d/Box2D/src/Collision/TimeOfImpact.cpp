/*
* Copyright (c) 2007-2009 Erin Catto http://www.box2d.org
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

#include <Box2D/Collision/Collision.hpp>
#include <Box2D/Collision/Distance.hpp>
#include <Box2D/Collision/TimeOfImpact.hpp>
#include <Box2D/Collision/Shapes/CircleShape.hpp>
#include <Box2D/Collision/Shapes/PolygonShape.hpp>
#include <Box2D/Common/Timer.hpp>

#include <cstdio>

b2::float32 b2_toiTime, b2_toiMaxTime;
b2::int32 b2_toiCalls, b2_toiIters, b2_toiMaxIters;
b2::int32 b2_toiRootIters, b2_toiMaxRootIters;

namespace b2
{

//
struct SeparationFunction
{
	enum Type
	{
		e_points,
		e_faceA,
		e_faceB
	};

	// TODO_ERIN might not need to return the separation

	float32 Initialize(const SimplexCache* cache,
		const DistanceProxy* proxyA, const Sweep& sweepA,
		const DistanceProxy* proxyB, const Sweep& sweepB,
		float32 t1)
	{
		m_proxyA = proxyA;
		m_proxyB = proxyB;
		int32 count = cache->count;
		assert(0 < count && count < 3);

		m_sweepA = sweepA;
		m_sweepB = sweepB;

		Transform xfA, xfB;
		m_sweepA.GetTransform(&xfA, t1);
		m_sweepB.GetTransform(&xfB, t1);

		if (count == 1)
		{
			m_type = e_points;
			Vec2 localPointA = m_proxyA->GetVertex(cache->indexA[0]);
			Vec2 localPointB = m_proxyB->GetVertex(cache->indexB[0]);
			Vec2 pointA = Mul(xfA, localPointA);
			Vec2 pointB = Mul(xfB, localPointB);
			m_axis = pointB - pointA;
			float32 s = m_axis.Normalize();
			return s;
		}
		else if (cache->indexA[0] == cache->indexA[1])
		{
			// Two points on B and one on A.
			m_type = e_faceB;
			Vec2 localPointB1 = proxyB->GetVertex(cache->indexB[0]);
			Vec2 localPointB2 = proxyB->GetVertex(cache->indexB[1]);

			m_axis = Cross(localPointB2 - localPointB1, 1.0f);
			m_axis.Normalize();
			Vec2 normal = Mul(xfB.q, m_axis);

			m_localPoint = 0.5f * (localPointB1 + localPointB2);
			Vec2 pointB = Mul(xfB, m_localPoint);

			Vec2 localPointA = proxyA->GetVertex(cache->indexA[0]);
			Vec2 pointA = Mul(xfA, localPointA);

			float32 s = Dot(pointA - pointB, normal);
			if (s < 0.0f)
			{
				m_axis = -m_axis;
				s = -s;
			}
			return s;
		}
		else
		{
			// Two points on A and one or two points on B.
			m_type = e_faceA;
			Vec2 localPointA1 = m_proxyA->GetVertex(cache->indexA[0]);
			Vec2 localPointA2 = m_proxyA->GetVertex(cache->indexA[1]);
			
			m_axis = Cross(localPointA2 - localPointA1, 1.0f);
			m_axis.Normalize();
			Vec2 normal = Mul(xfA.q, m_axis);

			m_localPoint = 0.5f * (localPointA1 + localPointA2);
			Vec2 pointA = Mul(xfA, m_localPoint);

			Vec2 localPointB = m_proxyB->GetVertex(cache->indexB[0]);
			Vec2 pointB = Mul(xfB, localPointB);

			float32 s = Dot(pointB - pointA, normal);
			if (s < 0.0f)
			{
				m_axis = -m_axis;
				s = -s;
			}
			return s;
		}
	}

	//
	float32 FindMinSeparation(int32* indexA, int32* indexB, float32 t) const
	{
		Transform xfA, xfB;
		m_sweepA.GetTransform(&xfA, t);
		m_sweepB.GetTransform(&xfB, t);

		switch (m_type)
		{
		case e_points:
			{
				Vec2 axisA = MulT(xfA.q,  m_axis);
				Vec2 axisB = MulT(xfB.q, -m_axis);

				*indexA = m_proxyA->GetSupport(axisA);
				*indexB = m_proxyB->GetSupport(axisB);

				Vec2 localPointA = m_proxyA->GetVertex(*indexA);
				Vec2 localPointB = m_proxyB->GetVertex(*indexB);
				
				Vec2 pointA = Mul(xfA, localPointA);
				Vec2 pointB = Mul(xfB, localPointB);

				float32 separation = Dot(pointB - pointA, m_axis);
				return separation;
			}

		case e_faceA:
			{
				Vec2 normal = Mul(xfA.q, m_axis);
				Vec2 pointA = Mul(xfA, m_localPoint);

				Vec2 axisB = MulT(xfB.q, -normal);
				
				*indexA = -1;
				*indexB = m_proxyB->GetSupport(axisB);

				Vec2 localPointB = m_proxyB->GetVertex(*indexB);
				Vec2 pointB = Mul(xfB, localPointB);

				float32 separation = Dot(pointB - pointA, normal);
				return separation;
			}

		case e_faceB:
			{
				Vec2 normal = Mul(xfB.q, m_axis);
				Vec2 pointB = Mul(xfB, m_localPoint);

				Vec2 axisA = MulT(xfA.q, -normal);

				*indexB = -1;
				*indexA = m_proxyA->GetSupport(axisA);

				Vec2 localPointA = m_proxyA->GetVertex(*indexA);
				Vec2 pointA = Mul(xfA, localPointA);

				float32 separation = Dot(pointA - pointB, normal);
				return separation;
			}

		default:
			assert(false);
			*indexA = -1;
			*indexB = -1;
			return 0.0f;
		}
	}

	//
	float32 Evaluate(int32 indexA, int32 indexB, float32 t) const
	{
		Transform xfA, xfB;
		m_sweepA.GetTransform(&xfA, t);
		m_sweepB.GetTransform(&xfB, t);

		switch (m_type)
		{
		case e_points:
			{
				Vec2 localPointA = m_proxyA->GetVertex(indexA);
				Vec2 localPointB = m_proxyB->GetVertex(indexB);

				Vec2 pointA = Mul(xfA, localPointA);
				Vec2 pointB = Mul(xfB, localPointB);
				float32 separation = Dot(pointB - pointA, m_axis);

				return separation;
			}

		case e_faceA:
			{
				Vec2 normal = Mul(xfA.q, m_axis);
				Vec2 pointA = Mul(xfA, m_localPoint);

				Vec2 localPointB = m_proxyB->GetVertex(indexB);
				Vec2 pointB = Mul(xfB, localPointB);

				float32 separation = Dot(pointB - pointA, normal);
				return separation;
			}

		case e_faceB:
			{
				Vec2 normal = Mul(xfB.q, m_axis);
				Vec2 pointB = Mul(xfB, m_localPoint);

				Vec2 localPointA = m_proxyA->GetVertex(indexA);
				Vec2 pointA = Mul(xfA, localPointA);

				float32 separation = Dot(pointA - pointB, normal);
				return separation;
			}

		default:
			assert(false);
			return 0.0f;
		}
	}

	const DistanceProxy* m_proxyA;
	const DistanceProxy* m_proxyB;
	Sweep m_sweepA, m_sweepB;
	Type m_type;
	Vec2 m_localPoint;
	Vec2 m_axis;
};

// CCD via the local separating axis method. This seeks progression
// by computing the largest time at which separation is maintained.
void TimeOfImpact(TOIOutput* output, const TOIInput* input)
{
	Timer timer;

	++b2_toiCalls;

	output->state = TOIOutput::e_unknown;
	output->t = input->tMax;

	const DistanceProxy* proxyA = &input->proxyA;
	const DistanceProxy* proxyB = &input->proxyB;

	Sweep sweepA = input->sweepA;
	Sweep sweepB = input->sweepB;

	// Large rotations can make the root finder fail, so we normalize the
	// sweep angles.
	sweepA.Normalize();
	sweepB.Normalize();

	float32 tMax = input->tMax;

	float32 totalRadius = proxyA->m_radius + proxyB->m_radius;
	float32 target = Max(linearSlop, totalRadius - 3.0f * linearSlop);
	float32 tolerance = 0.25f * linearSlop;
	assert(target > tolerance);

	float32 t1 = 0.0f;
	const int32 k_maxIterations = 20;	// TODO_ERIN b2::Settings
	int32 iter = 0;

	// Prepare input for distance query.
	SimplexCache cache;
	cache.count = 0;
	DistanceInput distanceInput;
	distanceInput.proxyA = input->proxyA;
	distanceInput.proxyB = input->proxyB;
	distanceInput.useRadii = false;

	// The outer loop progressively attempts to compute new separating axes.
	// This loop terminates when an axis is repeated (no progress is made).
	for(;;)
	{
		Transform xfA, xfB;
		sweepA.GetTransform(&xfA, t1);
		sweepB.GetTransform(&xfB, t1);

		// Get the distance between shapes. We can also use the results
		// to get a separating axis.
		distanceInput.transformA = xfA;
		distanceInput.transformB = xfB;
		DistanceOutput distanceOutput;
		Distance(&distanceOutput, &cache, &distanceInput);

		// If the shapes are overlapped, we give up on continuous collision.
		if (distanceOutput.distance <= 0.0f)
		{
			// Failure!
			output->state = TOIOutput::e_overlapped;
			output->t = 0.0f;
			break;
		}

		if (distanceOutput.distance < target + tolerance)
		{
			// Victory!
			output->state = TOIOutput::e_touching;
			output->t = t1;
			break;
		}

		// Initialize the separating axis.
		SeparationFunction fcn;
		fcn.Initialize(&cache, proxyA, sweepA, proxyB, sweepB, t1);
#if 0
		// Dump the curve seen by the root finder
		{
			const int32 N = 100;
			float32 dx = 1.0f / N;
			float32 xs[N+1];
			float32 fs[N+1];

			float32 x = 0.0f;

			for (int32 i = 0; i <= N; ++i)
			{
				sweepA.GetTransform(&xfA, x);
				sweepB.GetTransform(&xfB, x);
				float32 f = fcn.Evaluate(xfA, xfB) - target;

				std::printf("%g %g\n", x, f);

				xs[i] = x;
				fs[i] = f;

				x += dx;
			}
		}
#endif

		// Compute the TOI on the separating axis. We do this by successively
		// resolving the deepest point. This loop is bounded by the number of vertices.
		bool done = false;
		float32 t2 = tMax;
		int32 pushBackIter = 0;
		for (;;)
		{
			// Find the deepest point at t2. Store the witness point indices.
			int32 indexA, indexB;
			float32 s2 = fcn.FindMinSeparation(&indexA, &indexB, t2);

			// Is the final configuration separated?
			if (s2 > target + tolerance)
			{
				// Victory!
				output->state = TOIOutput::e_separated;
				output->t = tMax;
				done = true;
				break;
			}

			// Has the separation reached tolerance?
			if (s2 > target - tolerance)
			{
				// Advance the sweeps
				t1 = t2;
				break;
			}

			// Compute the initial separation of the witness points.
			float32 s1 = fcn.Evaluate(indexA, indexB, t1);

			// Check for initial overlap. This might happen if the root finder
			// runs out of iterations.
			if (s1 < target - tolerance)
			{
				output->state = TOIOutput::e_failed;
				output->t = t1;
				done = true;
				break;
			}

			// Check for touching
			if (s1 <= target + tolerance)
			{
				// Victory! t1 should hold the TOI (could be 0.0).
				output->state = TOIOutput::e_touching;
				output->t = t1;
				done = true;
				break;
			}

			// Compute 1D root of: f(x) - target = 0
			int32 rootIterCount = 0;
			float32 a1 = t1, a2 = t2;
			for (;;)
			{
				// Use a mix of the secant rule and bisection.
				float32 t;
				if (rootIterCount & 1)
				{
					// Secant rule to improve convergence.
					t = a1 + (target - s1) * (a2 - a1) / (s2 - s1);
				}
				else
				{
					// Bisection to guarantee progress.
					t = 0.5f * (a1 + a2);
				}

				++rootIterCount;
				++b2_toiRootIters;

				float32 s = fcn.Evaluate(indexA, indexB, t);

				if (Abs(s - target) < tolerance)
				{
					// t2 holds a tentative value for t1
					t2 = t;
					break;
				}

				// Ensure we continue to bracket the root.
				if (s > target)
				{
					a1 = t;
					s1 = s;
				}
				else
				{
					a2 = t;
					s2 = s;
				}
				
				if (rootIterCount == 50)
				{
					break;
				}
			}

			b2_toiMaxRootIters = Max(b2_toiMaxRootIters, rootIterCount);

			++pushBackIter;

			if (pushBackIter == maxPolygonVertices)
			{
				break;
			}
		}

		++iter;
		++b2_toiIters;

		if (done)
		{
			break;
		}

		if (iter == k_maxIterations)
		{
			// Root finder got stuck. Semi-victory.
			output->state = TOIOutput::e_failed;
			output->t = t1;
			break;
		}
	}

	b2_toiMaxIters = Max(b2_toiMaxIters, iter);

	float32 time = timer.GetMilliseconds();
	b2_toiMaxTime = Max(b2_toiMaxTime, time);
	b2_toiTime += time;
}

} // namespace b2
