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

#include <Box2D/Collision/Shapes/ChainShape.hpp>
#include <Box2D/Collision/Shapes/EdgeShape.hpp>
#include <new>
#include <cstring>

namespace b2
{

ChainShape::~ChainShape()
{
	Clear();
}

void ChainShape::Clear()
{
	Free(m_vertices);
	m_vertices = NULL;
	m_count = 0;
}

void ChainShape::CreateLoop(const Vec2* vertices, int32 count)
{
	assert(m_vertices == NULL && m_count == 0);
	assert(count >= 3);
	for (int32 i = 1; i < count; ++i)
	{
		Vec2 v1 = vertices[i-1];
		Vec2 v2 = vertices[i];
		// If the code crashes here, it means your vertices are too close together.
		assert(DistanceSquared(v1, v2) > linearSlop * linearSlop);
	}

	m_count = count + 1;
	m_vertices = (Vec2*)Alloc(m_count * sizeof(Vec2));
	std::memcpy(m_vertices, vertices, count * sizeof(Vec2));
	m_vertices[count] = m_vertices[0];
	m_prevVertex = m_vertices[m_count - 2];
	m_nextVertex = m_vertices[1];
	m_hasPrevVertex = true;
	m_hasNextVertex = true;
}

void ChainShape::CreateChain(const Vec2* vertices, int32 count)
{
	assert(m_vertices == NULL && m_count == 0);
	assert(count >= 2);
	for (int32 i = 1; i < count; ++i)
	{
		// If the code crashes here, it means your vertices are too close together.
		assert(DistanceSquared(vertices[i-1], vertices[i]) > linearSlop * linearSlop);
	}

	m_count = count;
	m_vertices = (Vec2*)Alloc(count * sizeof(Vec2));
	std::memcpy(m_vertices, vertices, m_count * sizeof(Vec2));

	m_hasPrevVertex = false;
	m_hasNextVertex = false;

	m_prevVertex.SetZero();
	m_nextVertex.SetZero();
}

void ChainShape::SetPrevVertex(const Vec2& prevVertex)
{
	m_prevVertex = prevVertex;
	m_hasPrevVertex = true;
}

void ChainShape::SetNextVertex(const Vec2& nextVertex)
{
	m_nextVertex = nextVertex;
	m_hasNextVertex = true;
}

Shape* ChainShape::Clone(BlockAllocator* allocator) const
{
	void* mem = allocator->Allocate(sizeof(ChainShape));
	ChainShape* clone = new (mem) ChainShape;
	clone->CreateChain(m_vertices, m_count);
	clone->m_prevVertex = m_prevVertex;
	clone->m_nextVertex = m_nextVertex;
	clone->m_hasPrevVertex = m_hasPrevVertex;
	clone->m_hasNextVertex = m_hasNextVertex;
	return clone;
}

int32 ChainShape::GetChildCount() const
{
	// edge count = vertex count - 1
	return m_count - 1;
}

void ChainShape::GetChildEdge(EdgeShape* edge, int32 index) const
{
	assert(0 <= index && index < m_count - 1);
	edge->m_type = Shape::e_edge;
	edge->m_radius = m_radius;

	edge->m_vertex1 = m_vertices[index + 0];
	edge->m_vertex2 = m_vertices[index + 1];

	if (index > 0)
	{
		edge->m_vertex0 = m_vertices[index - 1];
		edge->m_hasVertex0 = true;
	}
	else
	{
		edge->m_vertex0 = m_prevVertex;
		edge->m_hasVertex0 = m_hasPrevVertex;
	}

	if (index < m_count - 2)
	{
		edge->m_vertex3 = m_vertices[index + 2];
		edge->m_hasVertex3 = true;
	}
	else
	{
		edge->m_vertex3 = m_nextVertex;
		edge->m_hasVertex3 = m_hasNextVertex;
	}
}

bool ChainShape::TestPoint(const Transform& xf, const Vec2& p) const
{
	B2_NOT_USED(xf);
	B2_NOT_USED(p);
	return false;
}

bool ChainShape::RayCast(RayCastOutput* output, const RayCastInput& input,
						 const Transform& xf, int32 childIndex) const
{
	assert(childIndex < m_count);

	EdgeShape edgeShape;

	int32 i1 = childIndex;
	int32 i2 = childIndex + 1;
	if (i2 == m_count)
	{
		i2 = 0;
	}

	edgeShape.m_vertex1 = m_vertices[i1];
	edgeShape.m_vertex2 = m_vertices[i2];

	return edgeShape.RayCast(output, input, xf, 0);
}

void ChainShape::ComputeAABB(AABB* aabb, const Transform& xf, int32 childIndex) const
{
	assert(childIndex < m_count);

	int32 i1 = childIndex;
	int32 i2 = childIndex + 1;
	if (i2 == m_count)
	{
		i2 = 0;
	}

	Vec2 v1 = Mul(xf, m_vertices[i1]);
	Vec2 v2 = Mul(xf, m_vertices[i2]);

	aabb->lowerBound = Min(v1, v2);
	aabb->upperBound = Max(v1, v2);
}

void ChainShape::ComputeMass(MassData* massData, float32 density) const
{
	B2_NOT_USED(density);

	massData->mass = 0.0f;
	massData->center.SetZero();
	massData->I = 0.0f;
}

} // namespace b2
