/*
* Copyright (c) 2006-2010 Erin Catto http://www.box2d.org
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

#ifndef B2_CHAIN_SHAPE_HPP
#define B2_CHAIN_SHAPE_HPP

#include <Box2D/Collision/Shapes/Shape.hpp>

namespace b2
{

class EdgeShape;

/// A chain shape is a free form sequence of line segments.
/// The chain has two-sided collision, so you can use inside and outside collision.
/// Therefore, you may use any winding order.
/// Since there may be many vertices, they are allocated using b2::Alloc.
/// Connectivity information is used to create smooth collisions.
/// WARNING: The chain will not collide properly if there are self-intersections.
class ChainShape : public Shape
{
public:
	ChainShape();

	/// The destructor frees the vertices using b2::Free.
	~ChainShape();

	/// Clear all data.
	void Clear();

	/// Create a loop. This automatically adjusts connectivity.
	/// @param vertices an array of vertices, these are copied
	/// @param count the vertex count
	void CreateLoop(const Vec2* vertices, int32 count);

	/// Create a chain with isolated end vertices.
	/// @param vertices an array of vertices, these are copied
	/// @param count the vertex count
	void CreateChain(const Vec2* vertices, int32 count);

	/// Establish connectivity to a vertex that precedes the first vertex.
	/// Don't call this for loops.
	void SetPrevVertex(const Vec2& prevVertex);

	/// Establish connectivity to a vertex that follows the last vertex.
	/// Don't call this for loops.
	void SetNextVertex(const Vec2& nextVertex);

	/// Implement b2::Shape. Vertices are cloned using b2::Alloc.
	Shape* Clone(BlockAllocator* allocator) const;

	/// @see b2::Shape::GetChildCount
	int32 GetChildCount() const;

	/// Get a child edge.
	void GetChildEdge(EdgeShape* edge, int32 index) const;

	/// This always return false.
	/// @see b2::Shape::TestPoint
	bool TestPoint(const Transform& transform, const Vec2& p) const;

	/// Implement b2::Shape.
	bool RayCast(RayCastOutput* output, const RayCastInput& input,
					const Transform& transform, int32 childIndex) const;

	/// @see b2::Shape::ComputeAABB
	void ComputeAABB(AABB* aabb, const Transform& transform, int32 childIndex) const;

	/// Chains have zero mass.
	/// @see b2::Shape::ComputeMass
	void ComputeMass(MassData* massData, float32 density) const;

	/// The vertices. Owned by this class.
	Vec2* m_vertices;

	/// The vertex count.
	int32 m_count;

	Vec2 m_prevVertex, m_nextVertex;
	bool m_hasPrevVertex, m_hasNextVertex;
};

inline ChainShape::ChainShape()
{
	m_type = e_chain;
	m_radius = polygonRadius;
	m_vertices = NULL;
	m_count = 0;
	m_hasPrevVertex = false;
	m_hasNextVertex = false;
}

} // namespace b2

#endif // B2_CHAIN_SHAPE_HPP
