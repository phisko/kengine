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

#include <Box2D/Collision/Shapes/EdgeShape.hpp>
#include <new>

namespace b2
{

void EdgeShape::Set(const Vec2& v1, const Vec2& v2)
{
	m_vertex1 = v1;
	m_vertex2 = v2;
	m_hasVertex0 = false;
	m_hasVertex3 = false;
}

Shape* EdgeShape::Clone(BlockAllocator* allocator) const
{
	void* mem = allocator->Allocate(sizeof(EdgeShape));
	EdgeShape* clone = new (mem) EdgeShape;
	*clone = *this;
	return clone;
}

int32 EdgeShape::GetChildCount() const
{
	return 1;
}

bool EdgeShape::TestPoint(const Transform& xf, const Vec2& p) const
{
	B2_NOT_USED(xf);
	B2_NOT_USED(p);
	return false;
}

// p = p1 + t * d
// v = v1 + s * e
// p1 + t * d = v1 + s * e
// s * e - t * d = p1 - v1
bool EdgeShape::RayCast(RayCastOutput* output, const RayCastInput& input,
						const Transform& xf, int32 childIndex) const
{
	B2_NOT_USED(childIndex);

	// Put the ray into the edge's frame of reference.
	Vec2 p1 = MulT(xf.q, input.p1 - xf.p);
	Vec2 p2 = MulT(xf.q, input.p2 - xf.p);
	Vec2 d = p2 - p1;

	Vec2 v1 = m_vertex1;
	Vec2 v2 = m_vertex2;
	Vec2 e = v2 - v1;
	Vec2 normal(e.y, -e.x);
	normal.Normalize();

	// q = p1 + t * d
	// dot(normal, q - v1) = 0
	// dot(normal, p1 - v1) + t * dot(normal, d) = 0
	float32 numerator = Dot(normal, v1 - p1);
	float32 denominator = Dot(normal, d);

	if (denominator == 0.0f)
	{
		return false;
	}

	float32 t = numerator / denominator;
	if (t < 0.0f || input.maxFraction < t)
	{
		return false;
	}

	Vec2 q = p1 + t * d;

	// q = v1 + s * r
	// s = dot(q - v1, r) / dot(r, r)
	Vec2 r = v2 - v1;
	float32 rr = Dot(r, r);
	if (rr == 0.0f)
	{
		return false;
	}

	float32 s = Dot(q - v1, r) / rr;
	if (s < 0.0f || 1.0f < s)
	{
		return false;
	}

	output->fraction = t;
	if (numerator > 0.0f)
	{
		output->normal = -Mul(xf.q, normal);
	}
	else
	{
		output->normal = Mul(xf.q, normal);
	}
	return true;
}

void EdgeShape::ComputeAABB(AABB* aabb, const Transform& xf, int32 childIndex) const
{
	B2_NOT_USED(childIndex);

	Vec2 v1 = Mul(xf, m_vertex1);
	Vec2 v2 = Mul(xf, m_vertex2);

	Vec2 lower = Min(v1, v2);
	Vec2 upper = Max(v1, v2);

	Vec2 r(m_radius, m_radius);
	aabb->lowerBound = lower - r;
	aabb->upperBound = upper + r;
}

void EdgeShape::ComputeMass(MassData* massData, float32 density) const
{
	B2_NOT_USED(density);

	massData->mass = 0.0f;
	massData->center = 0.5f * (m_vertex1 + m_vertex2);
	massData->I = 0.0f;
}

} // namespace b2
