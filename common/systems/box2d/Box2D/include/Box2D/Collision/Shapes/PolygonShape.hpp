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

#ifndef B2_POLYGON_SHAPE_HPP
#define B2_POLYGON_SHAPE_HPP

#include <Box2D/Collision/Shapes/Shape.hpp>

namespace b2
{

/// A convex polygon. It is assumed that the interior of the polygon is to
/// the left of each edge.
/// Polygons have a maximum number of vertices equal to maxPolygonVertices.
/// In most cases you should not need many vertices for a convex polygon.
class PolygonShape : public Shape
{
public:
	PolygonShape();

	/// Implement b2::Shape.
	Shape* Clone(BlockAllocator* allocator) const;

	/// @see b2::Shape::GetChildCount
	int32 GetChildCount() const;

	/// Create a convex hull from the given array of local points.
	/// The count must be in the range [3, maxPolygonVertices].
	/// @warning the points may be re-ordered, even if they form a convex polygon
	/// @warning collinear points are handled but not removed. Collinear points
	/// may lead to poor stacking behavior.
	void Set(const Vec2* points, int32 count);

	/// Build vertices to represent an axis-aligned box centered on the local origin.
	/// @param hx the half-width.
	/// @param hy the half-height.
	void SetAsBox(float32 hx, float32 hy);

	/// Build vertices to represent an oriented box.
	/// @param hx the half-width.
	/// @param hy the half-height.
	/// @param center the center of the box in local coordinates.
	/// @param angle the rotation of the box in local coordinates.
	void SetAsBox(float32 hx, float32 hy, const Vec2& center, float32 angle);

	/// @see b2::Shape::TestPoint
	bool TestPoint(const Transform& transform, const Vec2& p) const;

	/// Implement b2::Shape.
	bool RayCast(RayCastOutput* output, const RayCastInput& input,
					const Transform& transform, int32 childIndex) const;

	/// @see b2::Shape::ComputeAABB
	void ComputeAABB(AABB* aabb, const Transform& transform, int32 childIndex) const;

	/// @see b2::Shape::ComputeMass
	void ComputeMass(MassData* massData, float32 density) const;

	/// Get the vertex count.
	int32 GetVertexCount() const { return m_count; }

	/// Get a vertex by index.
	const Vec2& GetVertex(int32 index) const;

	/// Validate convexity. This is a very time consuming operation.
	/// @returns true if valid
	bool Validate() const;

	Vec2 m_centroid;
	Vec2 m_vertices[maxPolygonVertices];
	Vec2 m_normals[maxPolygonVertices];
	int32 m_count;
};

inline PolygonShape::PolygonShape()
{
	m_type = e_polygon;
	m_radius = polygonRadius;
	m_count = 0;
	m_centroid.SetZero();
}

inline const Vec2& PolygonShape::GetVertex(int32 index) const
{
	assert(0 <= index && index < m_count);
	return m_vertices[index];
}

} // namespace b2

#endif // B2_POLYGON_SHAPE_HPP
