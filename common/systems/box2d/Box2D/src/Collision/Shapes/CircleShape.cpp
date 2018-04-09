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

#include <Box2D/Collision/Shapes/CircleShape.hpp>
#include <new>

namespace b2
{

Shape* CircleShape::Clone(BlockAllocator* allocator) const
{
	void* mem = allocator->Allocate(sizeof(CircleShape));
	CircleShape* clone = new (mem) CircleShape;
	*clone = *this;
	return clone;
}

int32 CircleShape::GetChildCount() const
{
	return 1;
}

bool CircleShape::TestPoint(const Transform& transform, const Vec2& p) const
{
	Vec2 center = transform.p + Mul(transform.q, m_p);
	Vec2 d = p - center;
	return Dot(d, d) <= m_radius * m_radius;
}

// Collision Detection in Interactive 3D Environments by Gino van den Bergen
// From Section 3.1.2
// x = s + a * r
// norm(x) = radius
bool CircleShape::RayCast(RayCastOutput* output, const RayCastInput& input,
						  const Transform& transform, int32 childIndex) const
{
	B2_NOT_USED(childIndex);

	Vec2 position = transform.p + Mul(transform.q, m_p);
	Vec2 s = input.p1 - position;
	float32 b = Dot(s, s) - m_radius * m_radius;

	// Solve quadratic equation.
	Vec2 r = input.p2 - input.p1;
	float32 c =  Dot(s, r);
	float32 rr = Dot(r, r);
	float32 sigma = c * c - rr * b;

	// Check for negative discriminant and short segment.
	if (sigma < 0.0f || rr < epsilon)
	{
		return false;
	}

	// Find the point of intersection of the line with the circle.
	float32 a = -(c + sqrtf(sigma));

	// Is the intersection point on the segment?
	if (0.0f <= a && a <= input.maxFraction * rr)
	{
		a /= rr;
		output->fraction = a;
		output->normal = s + a * r;
		output->normal.Normalize();
		return true;
	}

	return false;
}

void CircleShape::ComputeAABB(AABB* aabb, const Transform& transform, int32 childIndex) const
{
	B2_NOT_USED(childIndex);

	Vec2 p = transform.p + Mul(transform.q, m_p);
	aabb->lowerBound.Set(p.x - m_radius, p.y - m_radius);
	aabb->upperBound.Set(p.x + m_radius, p.y + m_radius);
}

void CircleShape::ComputeMass(MassData* massData, float32 density) const
{
	massData->mass = density * pi * m_radius * m_radius;
	massData->center = m_p;

	// inertia about the local origin
	massData->I = massData->mass * (0.5f * m_radius * m_radius + Dot(m_p, m_p));
}

} // namespace b2
