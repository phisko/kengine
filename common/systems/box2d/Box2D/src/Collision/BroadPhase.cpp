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

#include <Box2D/Collision/BroadPhase.hpp>

namespace b2
{

BroadPhase::BroadPhase()
{
	m_proxyCount = 0;

	m_pairCapacity = 16;
	m_pairCount = 0;
	m_pairBuffer = (Pair*)Alloc(m_pairCapacity * sizeof(Pair));

	m_moveCapacity = 16;
	m_moveCount = 0;
	m_moveBuffer = (int32*)Alloc(m_moveCapacity * sizeof(int32));
}

BroadPhase::~BroadPhase()
{
	Free(m_moveBuffer);
	Free(m_pairBuffer);
}

int32 BroadPhase::CreateProxy(const AABB& aabb, void* userData)
{
	int32 proxyId = m_tree.CreateProxy(aabb, userData);
	++m_proxyCount;
	BufferMove(proxyId);
	return proxyId;
}

void BroadPhase::DestroyProxy(int32 proxyId)
{
	UnBufferMove(proxyId);
	--m_proxyCount;
	m_tree.DestroyProxy(proxyId);
}

void BroadPhase::MoveProxy(int32 proxyId, const AABB& aabb, const Vec2& displacement)
{
	bool buffer = m_tree.MoveProxy(proxyId, aabb, displacement);
	if (buffer)
	{
		BufferMove(proxyId);
	}
}

void BroadPhase::TouchProxy(int32 proxyId)
{
	BufferMove(proxyId);
}

void BroadPhase::BufferMove(int32 proxyId)
{
	if (m_moveCount == m_moveCapacity)
	{
		int32* oldBuffer = m_moveBuffer;
		m_moveCapacity *= 2;
		m_moveBuffer = (int32*)Alloc(m_moveCapacity * sizeof(int32));
		memcpy(m_moveBuffer, oldBuffer, m_moveCount * sizeof(int32));
		Free(oldBuffer);
	}

	m_moveBuffer[m_moveCount] = proxyId;
	++m_moveCount;
}

void BroadPhase::UnBufferMove(int32 proxyId)
{
	for (int32 i = 0; i < m_moveCount; ++i)
	{
		if (m_moveBuffer[i] == proxyId)
		{
			m_moveBuffer[i] = e_nullProxy;
		}
	}
}

// This is called from b2::DynamicTree::Query when we are gathering pairs.
bool BroadPhase::QueryCallback(int32 proxyId)
{
	// A proxy cannot form a pair with itself.
	if (proxyId == m_queryProxyId)
	{
		return true;
	}

	// Grow the pair buffer as needed.
	if (m_pairCount == m_pairCapacity)
	{
		Pair* oldBuffer = m_pairBuffer;
		m_pairCapacity *= 2;
		m_pairBuffer = (Pair*)Alloc(m_pairCapacity * sizeof(Pair));
		memcpy(m_pairBuffer, oldBuffer, m_pairCount * sizeof(Pair));
		Free(oldBuffer);
	}

	m_pairBuffer[m_pairCount].proxyIdA = Min(proxyId, m_queryProxyId);
	m_pairBuffer[m_pairCount].proxyIdB = Max(proxyId, m_queryProxyId);
	++m_pairCount;

	return true;
}

} // namespace b2
