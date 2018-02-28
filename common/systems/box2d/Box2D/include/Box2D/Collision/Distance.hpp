
/*
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
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

#ifndef B2_DISTANCE_HPP
#define B2_DISTANCE_HPP

#include <Box2D/Common/Math.hpp>

namespace b2
{

class Shape;

/// A distance proxy is used by the GJK algorithm.
/// It encapsulates any shape.
struct DistanceProxy
{
	DistanceProxy() : m_vertices(NULL), m_count(0), m_radius(0.0f) {}

	/// Initialize the proxy using the given shape. The shape
	/// must remain in scope while the proxy is in use.
	void Set(const Shape* shape, int32 index);

	/// Get the supporting vertex index in the given direction.
	int32 GetSupport(const Vec2& d) const;

	/// Get the supporting vertex in the given direction.
	const Vec2& GetSupportVertex(const Vec2& d) const;

	/// Get the vertex count.
	int32 GetVertexCount() const;

	/// Get a vertex by index. Used by b2::Distance.
	const Vec2& GetVertex(int32 index) const;

	Vec2 m_buffer[2];
	const Vec2* m_vertices;
	int32 m_count;
	float32 m_radius;
};

/// Used to warm start b2::Distance.
/// Set count to zero on first call.
struct SimplexCache
{
	float32 metric;		///< length or area
	uint16 count;
	uint8 indexA[3];	///< vertices on shape A
	uint8 indexB[3];	///< vertices on shape B
};

/// Input for b2::Distance.
/// You have to option to use the shape radii
/// in the computation. Even 
struct DistanceInput
{
	DistanceProxy proxyA;
	DistanceProxy proxyB;
	Transform transformA;
	Transform transformB;
	bool useRadii;
};

/// Output for b2::Distance.
struct DistanceOutput
{
	Vec2 pointA;		///< closest point on shapeA
	Vec2 pointB;		///< closest point on shapeB
	float32 distance;
	int32 iterations;	///< number of GJK iterations used
};

/// Compute the closest points between two shapes. Supports any combination of:
/// b2::CircleShape, b2::PolygonShape, b2::EdgeShape. The simplex cache is input/output.
/// On the first call set b2::SimplexCache.count to zero.
void Distance(DistanceOutput* output,
				SimplexCache* cache, 
				const DistanceInput* input);


//////////////////////////////////////////////////////////////////////////

inline int32 DistanceProxy::GetVertexCount() const
{
	return m_count;
}

inline const Vec2& DistanceProxy::GetVertex(int32 index) const
{
	assert(0 <= index && index < m_count);
	return m_vertices[index];
}

inline int32 DistanceProxy::GetSupport(const Vec2& d) const
{
	int32 bestIndex = 0;
	float32 bestValue = Dot(m_vertices[0], d);
	for (int32 i = 1; i < m_count; ++i)
	{
		float32 value = Dot(m_vertices[i], d);
		if (value > bestValue)
		{
			bestIndex = i;
			bestValue = value;
		}
	}

	return bestIndex;
}

inline const Vec2& DistanceProxy::GetSupportVertex(const Vec2& d) const
{
	int32 bestIndex = 0;
	float32 bestValue = Dot(m_vertices[0], d);
	for (int32 i = 1; i < m_count; ++i)
	{
		float32 value = Dot(m_vertices[i], d);
		if (value > bestValue)
		{
			bestIndex = i;
			bestValue = value;
		}
	}

	return m_vertices[bestIndex];
}

} // namespace b2

#endif // B2_DISTANCE_HPP
