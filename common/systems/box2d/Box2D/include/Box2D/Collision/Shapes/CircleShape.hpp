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

#ifndef B2_CIRCLE_SHAPE_HPP
#define B2_CIRCLE_SHAPE_HPP

#include <Box2D/Collision/Shapes/Shape.hpp>

namespace b2
{

/// A circle shape.
class CircleShape : public Shape
{
public:
	CircleShape();

	/// Implement b2::Shape.
	Shape* Clone(BlockAllocator* allocator) const;

	/// @see b2::Shape::GetChildCount
	int32 GetChildCount() const;

	/// Implement b2::Shape.
	bool TestPoint(const Transform& transform, const Vec2& p) const;

	/// Implement b2::Shape.
	bool RayCast(RayCastOutput* output, const RayCastInput& input,
				const Transform& transform, int32 childIndex) const;

	/// @see b2::Shape::ComputeAABB
	void ComputeAABB(AABB* aabb, const Transform& transform, int32 childIndex) const;

	/// @see b2::Shape::ComputeMass
	void ComputeMass(MassData* massData, float32 density) const;

	/// Get the supporting vertex index in the given direction.
	int32 GetSupport(const Vec2& d) const;

	/// Get the supporting vertex in the given direction.
	const Vec2& GetSupportVertex(const Vec2& d) const;

	/// Get the vertex count.
	int32 GetVertexCount() const { return 1; }

	/// Get a vertex by index. Used by b2::Distance.
	const Vec2& GetVertex(int32 index) const;

	/// Position
	Vec2 m_p;
};

inline CircleShape::CircleShape()
{
	m_type = e_circle;
	m_radius = 0.0f;
	m_p.SetZero();
}

inline int32 CircleShape::GetSupport(const Vec2 &d) const
{
	B2_NOT_USED(d);
	return 0;
}

inline const Vec2& CircleShape::GetSupportVertex(const Vec2 &d) const
{
	B2_NOT_USED(d);
	return m_p;
}

inline const Vec2& CircleShape::GetVertex(int32 index) const
{
	B2_NOT_USED(index);
	assert(index == 0);
	return m_p;
}

} // namespace b2

#endif // B2_CIRCLE_SHAPE_HPP
