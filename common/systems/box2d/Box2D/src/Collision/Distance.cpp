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

#include <Box2D/Collision/Distance.hpp>
#include <Box2D/Collision/Shapes/CircleShape.hpp>
#include <Box2D/Collision/Shapes/EdgeShape.hpp>
#include <Box2D/Collision/Shapes/ChainShape.hpp>
#include <Box2D/Collision/Shapes/PolygonShape.hpp>

// GJK using Voronoi regions (Christer Ericson) and Barycentric coordinates.
b2::int32 b2_gjkCalls, b2_gjkIters, b2_gjkMaxIters;

namespace b2
{

void DistanceProxy::Set(const Shape* shape, int32 index)
{
	switch (shape->GetType())
	{
	case Shape::e_circle:
		{
			const CircleShape* circle = static_cast<const CircleShape*>(shape);
			m_vertices = &circle->m_p;
			m_count = 1;
			m_radius = circle->m_radius;
		}
		break;

	case Shape::e_polygon:
		{
			const PolygonShape* polygon = static_cast<const PolygonShape*>(shape);
			m_vertices = polygon->m_vertices;
			m_count = polygon->m_count;
			m_radius = polygon->m_radius;
		}
		break;

	case Shape::e_chain:
		{
			const ChainShape* chain = static_cast<const ChainShape*>(shape);
			assert(0 <= index && index < chain->m_count);

			m_buffer[0] = chain->m_vertices[index];
			if (index + 1 < chain->m_count)
			{
				m_buffer[1] = chain->m_vertices[index + 1];
			}
			else
			{
				m_buffer[1] = chain->m_vertices[0];
			}

			m_vertices = m_buffer;
			m_count = 2;
			m_radius = chain->m_radius;
		}
		break;

	case Shape::e_edge:
		{
			const EdgeShape* edge = static_cast<const EdgeShape*>(shape);
			m_vertices = &edge->m_vertex1;
			m_count = 2;
			m_radius = edge->m_radius;
		}
		break;

	default:
		assert(false);
	}
}


struct SimplexVertex
{
	Vec2 wA;		// support point in proxyA
	Vec2 wB;		// support point in proxyB
	Vec2 w;		// wB - wA
	float32 a;		// barycentric coordinate for closest point
	int32 indexA;	// wA index
	int32 indexB;	// wB index
};

struct Simplex
{
	void ReadCache(	const SimplexCache* cache,
					const DistanceProxy* proxyA, const Transform& transformA,
					const DistanceProxy* proxyB, const Transform& transformB)
	{
		assert(cache->count <= 3);
		
		// Copy data from cache.
		m_count = cache->count;
		SimplexVertex* vertices = &m_v1;
		for (int32 i = 0; i < m_count; ++i)
		{
			SimplexVertex* v = vertices + i;
			v->indexA = cache->indexA[i];
			v->indexB = cache->indexB[i];
			Vec2 wALocal = proxyA->GetVertex(v->indexA);
			Vec2 wBLocal = proxyB->GetVertex(v->indexB);
			v->wA = Mul(transformA, wALocal);
			v->wB = Mul(transformB, wBLocal);
			v->w = v->wB - v->wA;
			v->a = 0.0f;
		}

		// Compute the new simplex metric, if it is substantially different than
		// old metric then flush the simplex.
		if (m_count > 1)
		{
			float32 metric1 = cache->metric;
			float32 metric2 = GetMetric();
			if (metric2 < 0.5f * metric1 || 2.0f * metric1 < metric2 || metric2 < epsilon)
			{
				// Reset the simplex.
				m_count = 0;
			}
		}

		// If the cache is empty or invalid ...
		if (m_count == 0)
		{
			SimplexVertex* v = vertices + 0;
			v->indexA = 0;
			v->indexB = 0;
			Vec2 wALocal = proxyA->GetVertex(0);
			Vec2 wBLocal = proxyB->GetVertex(0);
			v->wA = Mul(transformA, wALocal);
			v->wB = Mul(transformB, wBLocal);
			v->w = v->wB - v->wA;
			v->a = 1.0f;
			m_count = 1;
		}
	}

	void WriteCache(SimplexCache* cache) const
	{
		cache->metric = GetMetric();
		cache->count = uint16(m_count);
		const SimplexVertex* vertices = &m_v1;
		for (int32 i = 0; i < m_count; ++i)
		{
			cache->indexA[i] = uint8(vertices[i].indexA);
			cache->indexB[i] = uint8(vertices[i].indexB);
		}
	}

	Vec2 GetSearchDirection() const
	{
		switch (m_count)
		{
		case 1:
			return -m_v1.w;

		case 2:
			{
				Vec2 e12 = m_v2.w - m_v1.w;
				float32 sgn = Cross(e12, -m_v1.w);
				if (sgn > 0.0f)
				{
					// Origin is left of e12.
					return Cross(1.0f, e12);
				}
				else
				{
					// Origin is right of e12.
					return Cross(e12, 1.0f);
				}
			}

		default:
			assert(false);
			return Vec2_zero;
		}
	}

	Vec2 GetClosestPoint() const
	{
		switch (m_count)
		{
		case 0:
			assert(false);
			return Vec2_zero;

		case 1:
			return m_v1.w;

		case 2:
			return m_v1.a * m_v1.w + m_v2.a * m_v2.w;

		case 3:
			return Vec2_zero;

		default:
			assert(false);
			return Vec2_zero;
		}
	}

	void GetWitnessPoints(Vec2* pA, Vec2* pB) const
	{
		switch (m_count)
		{
		case 0:
			assert(false);
			break;

		case 1:
			*pA = m_v1.wA;
			*pB = m_v1.wB;
			break;

		case 2:
			*pA = m_v1.a * m_v1.wA + m_v2.a * m_v2.wA;
			*pB = m_v1.a * m_v1.wB + m_v2.a * m_v2.wB;
			break;

		case 3:
			*pA = m_v1.a * m_v1.wA + m_v2.a * m_v2.wA + m_v3.a * m_v3.wA;
			*pB = *pA;
			break;

		default:
			assert(false);
			break;
		}
	}

	float32 GetMetric() const
	{
		switch (m_count)
		{
		case 0:
			assert(false);
			return 0.0f;

		case 1:
			return 0.0f;

		case 2:
			return Distance(m_v1.w, m_v2.w);

		case 3:
			return Cross(m_v2.w - m_v1.w, m_v3.w - m_v1.w);

		default:
			assert(false);
			return 0.0f;
		}
	}

	void Solve2();
	void Solve3();

	SimplexVertex m_v1, m_v2, m_v3;
	int32 m_count;
};


// Solve a line segment using barycentric coordinates.
//
// p = a1 * w1 + a2 * w2
// a1 + a2 = 1
//
// The vector from the origin to the closest point on the line is
// perpendicular to the line.
// e12 = w2 - w1
// dot(p, e) = 0
// a1 * dot(w1, e) + a2 * dot(w2, e) = 0
//
// 2-by-2 linear system
// [1      1     ][a1] = [1]
// [w1.e12 w2.e12][a2] = [0]
//
// Define
// d12_1 =  dot(w2, e12)
// d12_2 = -dot(w1, e12)
// d12 = d12_1 + d12_2
//
// Solution
// a1 = d12_1 / d12
// a2 = d12_2 / d12
void Simplex::Solve2()
{
	Vec2 w1 = m_v1.w;
	Vec2 w2 = m_v2.w;
	Vec2 e12 = w2 - w1;

	// w1 region
	float32 d12_2 = -Dot(w1, e12);
	if (d12_2 <= 0.0f)
	{
		// a2 <= 0, so we clamp it to 0
		m_v1.a = 1.0f;
		m_count = 1;
		return;
	}

	// w2 region
	float32 d12_1 = Dot(w2, e12);
	if (d12_1 <= 0.0f)
	{
		// a1 <= 0, so we clamp it to 0
		m_v2.a = 1.0f;
		m_count = 1;
		m_v1 = m_v2;
		return;
	}

	// Must be in e12 region.
	float32 inv_d12 = 1.0f / (d12_1 + d12_2);
	m_v1.a = d12_1 * inv_d12;
	m_v2.a = d12_2 * inv_d12;
	m_count = 2;
}

// Possible regions:
// - points[2]
// - edge points[0]-points[2]
// - edge points[1]-points[2]
// - inside the triangle
void Simplex::Solve3()
{
	Vec2 w1 = m_v1.w;
	Vec2 w2 = m_v2.w;
	Vec2 w3 = m_v3.w;

	// Edge12
	// [1      1     ][a1] = [1]
	// [w1.e12 w2.e12][a2] = [0]
	// a3 = 0
	Vec2 e12 = w2 - w1;
	float32 w1e12 = Dot(w1, e12);
	float32 w2e12 = Dot(w2, e12);
	float32 d12_1 = w2e12;
	float32 d12_2 = -w1e12;

	// Edge13
	// [1      1     ][a1] = [1]
	// [w1.e13 w3.e13][a3] = [0]
	// a2 = 0
	Vec2 e13 = w3 - w1;
	float32 w1e13 = Dot(w1, e13);
	float32 w3e13 = Dot(w3, e13);
	float32 d13_1 = w3e13;
	float32 d13_2 = -w1e13;

	// Edge23
	// [1      1     ][a2] = [1]
	// [w2.e23 w3.e23][a3] = [0]
	// a1 = 0
	Vec2 e23 = w3 - w2;
	float32 w2e23 = Dot(w2, e23);
	float32 w3e23 = Dot(w3, e23);
	float32 d23_1 = w3e23;
	float32 d23_2 = -w2e23;
	
	// Triangle123
	float32 n123 = Cross(e12, e13);

	float32 d123_1 = n123 * Cross(w2, w3);
	float32 d123_2 = n123 * Cross(w3, w1);
	float32 d123_3 = n123 * Cross(w1, w2);

	// w1 region
	if (d12_2 <= 0.0f && d13_2 <= 0.0f)
	{
		m_v1.a = 1.0f;
		m_count = 1;
		return;
	}

	// e12
	if (d12_1 > 0.0f && d12_2 > 0.0f && d123_3 <= 0.0f)
	{
		float32 inv_d12 = 1.0f / (d12_1 + d12_2);
		m_v1.a = d12_1 * inv_d12;
		m_v2.a = d12_2 * inv_d12;
		m_count = 2;
		return;
	}

	// e13
	if (d13_1 > 0.0f && d13_2 > 0.0f && d123_2 <= 0.0f)
	{
		float32 inv_d13 = 1.0f / (d13_1 + d13_2);
		m_v1.a = d13_1 * inv_d13;
		m_v3.a = d13_2 * inv_d13;
		m_count = 2;
		m_v2 = m_v3;
		return;
	}

	// w2 region
	if (d12_1 <= 0.0f && d23_2 <= 0.0f)
	{
		m_v2.a = 1.0f;
		m_count = 1;
		m_v1 = m_v2;
		return;
	}

	// w3 region
	if (d13_1 <= 0.0f && d23_1 <= 0.0f)
	{
		m_v3.a = 1.0f;
		m_count = 1;
		m_v1 = m_v3;
		return;
	}

	// e23
	if (d23_1 > 0.0f && d23_2 > 0.0f && d123_1 <= 0.0f)
	{
		float32 inv_d23 = 1.0f / (d23_1 + d23_2);
		m_v2.a = d23_1 * inv_d23;
		m_v3.a = d23_2 * inv_d23;
		m_count = 2;
		m_v1 = m_v3;
		return;
	}

	// Must be in triangle123
	float32 inv_d123 = 1.0f / (d123_1 + d123_2 + d123_3);
	m_v1.a = d123_1 * inv_d123;
	m_v2.a = d123_2 * inv_d123;
	m_v3.a = d123_3 * inv_d123;
	m_count = 3;
}

void Distance(DistanceOutput* output,
			  SimplexCache* cache,
			  const DistanceInput* input)
{
	++b2_gjkCalls;

	const DistanceProxy* proxyA = &input->proxyA;
	const DistanceProxy* proxyB = &input->proxyB;

	Transform transformA = input->transformA;
	Transform transformB = input->transformB;

	// Initialize the simplex.
	Simplex simplex;
	simplex.ReadCache(cache, proxyA, transformA, proxyB, transformB);

	// Get simplex vertices as an array.
	SimplexVertex* vertices = &simplex.m_v1;
	const int32 k_maxIters = 20;

	// These store the vertices of the last simplex so that we
	// can check for duplicates and prevent cycling.
	int32 saveA[3], saveB[3];
	int32 saveCount = 0;

	float32 distanceSqr1 = maxFloat;
	float32 distanceSqr2 = distanceSqr1;

	// Main iteration loop.
	int32 iter = 0;
	while (iter < k_maxIters)
	{
		// Copy simplex so we can identify duplicates.
		saveCount = simplex.m_count;
		for (int32 i = 0; i < saveCount; ++i)
		{
			saveA[i] = vertices[i].indexA;
			saveB[i] = vertices[i].indexB;
		}

		switch (simplex.m_count)
		{
		case 1:
			break;

		case 2:
			simplex.Solve2();
			break;

		case 3:
			simplex.Solve3();
			break;

		default:
			assert(false);
		}

		// If we have 3 points, then the origin is in the corresponding triangle.
		if (simplex.m_count == 3)
		{
			break;
		}

		// Compute closest point.
		Vec2 p = simplex.GetClosestPoint();
		distanceSqr2 = p.LengthSquared();

		// Ensure progress
		if (distanceSqr2 >= distanceSqr1)
		{
			//break;
		}
		distanceSqr1 = distanceSqr2;

		// Get search direction.
		Vec2 d = simplex.GetSearchDirection();

		// Ensure the search direction is numerically fit.
		if (d.LengthSquared() < epsilon * epsilon)
		{
			// The origin is probably contained by a line segment
			// or triangle. Thus the shapes are overlapped.

			// We can't return zero here even though there may be overlap.
			// In case the simplex is a point, segment, or triangle it is difficult
			// to determine if the origin is contained in the CSO or very close to it.
			break;
		}

		// Compute a tentative new simplex vertex using support points.
		SimplexVertex* vertex = vertices + simplex.m_count;
		vertex->indexA = proxyA->GetSupport(MulT(transformA.q, -d));
		vertex->wA = Mul(transformA, proxyA->GetVertex(vertex->indexA));
		Vec2 wBLocal;
		vertex->indexB = proxyB->GetSupport(MulT(transformB.q, d));
		vertex->wB = Mul(transformB, proxyB->GetVertex(vertex->indexB));
		vertex->w = vertex->wB - vertex->wA;

		// Iteration count is equated to the number of support point calls.
		++iter;
		++b2_gjkIters;

		// Check for duplicate support points. This is the main termination criteria.
		bool duplicate = false;
		for (int32 i = 0; i < saveCount; ++i)
		{
			if (vertex->indexA == saveA[i] && vertex->indexB == saveB[i])
			{
				duplicate = true;
				break;
			}
		}

		// If we found a duplicate support point we must exit to avoid cycling.
		if (duplicate)
		{
			break;
		}

		// New vertex is ok and needed.
		++simplex.m_count;
	}

	b2_gjkMaxIters = Max(b2_gjkMaxIters, iter);

	// Prepare output.
	simplex.GetWitnessPoints(&output->pointA, &output->pointB);
	output->distance = Distance(output->pointA, output->pointB);
	output->iterations = iter;

	// Cache the simplex.
	simplex.WriteCache(cache);

	// Apply radii if requested.
	if (input->useRadii)
	{
		float32 rA = proxyA->m_radius;
		float32 rB = proxyB->m_radius;

		if (output->distance > rA + rB && output->distance > epsilon)
		{
			// Shapes are still no overlapped.
			// Move the witness points to the outer surface.
			output->distance -= rA + rB;
			Vec2 normal = output->pointB - output->pointA;
			normal.Normalize();
			output->pointA += rA * normal;
			output->pointB -= rB * normal;
		}
		else
		{
			// Shapes are overlapped when radii are considered.
			// Move the witness points to the middle.
			Vec2 p = 0.5f * (output->pointA + output->pointB);
			output->pointA = p;
			output->pointB = p;
			output->distance = 0.0f;
		}
	}
}

} // namespace b2
