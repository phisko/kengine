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
#include <Box2D/Collision/Shapes/CircleShape.hpp>
#include <Box2D/Collision/Shapes/PolygonShape.hpp>

namespace b2
{

void CollideCircles(
	Manifold* manifold,
	const CircleShape* circleA, const Transform& xfA,
	const CircleShape* circleB, const Transform& xfB)
{
	manifold->pointCount = 0;

	Vec2 pA = Mul(xfA, circleA->m_p);
	Vec2 pB = Mul(xfB, circleB->m_p);

	Vec2 d = pB - pA;
	float32 distSqr = Dot(d, d);
	float32 rA = circleA->m_radius, rB = circleB->m_radius;
	float32 radius = rA + rB;
	if (distSqr > radius * radius)
	{
		return;
	}

	manifold->type = Manifold::e_circles;
	manifold->localPoint = circleA->m_p;
	manifold->localNormal.SetZero();
	manifold->pointCount = 1;

	manifold->points[0].localPoint = circleB->m_p;
	manifold->points[0].id.key = 0;
}

void CollidePolygonAndCircle(
	Manifold* manifold,
	const PolygonShape* polygonA, const Transform& xfA,
	const CircleShape* circleB, const Transform& xfB)
{
	manifold->pointCount = 0;

	// Compute circle position in the frame of the polygon.
	Vec2 c = Mul(xfB, circleB->m_p);
	Vec2 cLocal = MulT(xfA, c);

	// Find the min separating edge.
	int32 normalIndex = 0;
	float32 separation = -maxFloat;
	float32 radius = polygonA->m_radius + circleB->m_radius;
	int32 vertexCount = polygonA->m_count;
	const Vec2* vertices = polygonA->m_vertices;
	const Vec2* normals = polygonA->m_normals;

	for (int32 i = 0; i < vertexCount; ++i)
	{
		float32 s = Dot(normals[i], cLocal - vertices[i]);

		if (s > radius)
		{
			// Early out.
			return;
		}

		if (s > separation)
		{
			separation = s;
			normalIndex = i;
		}
	}

	// Vertices that subtend the incident face.
	int32 vertIndex1 = normalIndex;
	int32 vertIndex2 = vertIndex1 + 1 < vertexCount ? vertIndex1 + 1 : 0;
	Vec2 v1 = vertices[vertIndex1];
	Vec2 v2 = vertices[vertIndex2];

	// If the center is inside the polygon ...
	if (separation < epsilon)
	{
		manifold->pointCount = 1;
		manifold->type = Manifold::e_faceA;
		manifold->localNormal = normals[normalIndex];
		manifold->localPoint = 0.5f * (v1 + v2);
		manifold->points[0].localPoint = circleB->m_p;
		manifold->points[0].id.key = 0;
		return;
	}

	// Compute barycentric coordinates
	float32 u1 = Dot(cLocal - v1, v2 - v1);
	float32 u2 = Dot(cLocal - v2, v1 - v2);
	if (u1 <= 0.0f)
	{
		if (DistanceSquared(cLocal, v1) > radius * radius)
		{
			return;
		}

		manifold->pointCount = 1;
		manifold->type = Manifold::e_faceA;
		manifold->localNormal = cLocal - v1;
		manifold->localNormal.Normalize();
		manifold->localPoint = v1;
		manifold->points[0].localPoint = circleB->m_p;
		manifold->points[0].id.key = 0;
	}
	else if (u2 <= 0.0f)
	{
		if (DistanceSquared(cLocal, v2) > radius * radius)
		{
			return;
		}

		manifold->pointCount = 1;
		manifold->type = Manifold::e_faceA;
		manifold->localNormal = cLocal - v2;
		manifold->localNormal.Normalize();
		manifold->localPoint = v2;
		manifold->points[0].localPoint = circleB->m_p;
		manifold->points[0].id.key = 0;
	}
	else
	{
		Vec2 faceCenter = 0.5f * (v1 + v2);
		float32 separation = Dot(cLocal - faceCenter, normals[vertIndex1]);
		if (separation > radius)
		{
			return;
		}

		manifold->pointCount = 1;
		manifold->type = Manifold::e_faceA;
		manifold->localNormal = normals[vertIndex1];
		manifold->localPoint = faceCenter;
		manifold->points[0].localPoint = circleB->m_p;
		manifold->points[0].id.key = 0;
	}
}

} // namespace b2
