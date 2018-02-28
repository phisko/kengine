/*
* Copyright (c) 2011 Erin Catto http://box2d.org
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

#include <Box2D/Rope/Rope.hpp>
#include <Box2D/Common/Draw.hpp>

namespace b2
{

Rope::Rope()
{
	m_count = 0;
	m_ps = NULL;
	m_p0s = NULL;
	m_vs = NULL;
	m_ims = NULL;
	m_Ls = NULL;
	m_as = NULL;
	m_gravity.SetZero();
	m_k2 = 1.0f;
	m_k3 = 0.1f;
}

Rope::~Rope()
{
	Free(m_ps);
	Free(m_p0s);
	Free(m_vs);
	Free(m_ims);
	Free(m_Ls);
	Free(m_as);
}

void Rope::Initialize(const RopeDef* def)
{
	assert(def->count >= 3);
	m_count = def->count;
	m_ps = (Vec2*)Alloc(m_count * sizeof(Vec2));
	m_p0s = (Vec2*)Alloc(m_count * sizeof(Vec2));
	m_vs = (Vec2*)Alloc(m_count * sizeof(Vec2));
	m_ims = (float32*)Alloc(m_count * sizeof(float32));

	for (int32 i = 0; i < m_count; ++i)
	{
		m_ps[i] = def->vertices[i];
		m_p0s[i] = def->vertices[i];
		m_vs[i].SetZero();

		float32 m = def->masses[i];
		if (m > 0.0f)
		{
			m_ims[i] = 1.0f / m;
		}
		else
		{
			m_ims[i] = 0.0f;
		}
	}

	int32 count2 = m_count - 1;
	int32 count3 = m_count - 2;
	m_Ls = (float32*)Alloc(count2 * sizeof(float32));
	m_as = (float32*)Alloc(count3 * sizeof(float32));

	for (int32 i = 0; i < count2; ++i)
	{
		Vec2 p1 = m_ps[i];
		Vec2 p2 = m_ps[i+1];
		m_Ls[i] = Distance(p1, p2);
	}

	for (int32 i = 0; i < count3; ++i)
	{
		Vec2 p1 = m_ps[i];
		Vec2 p2 = m_ps[i + 1];
		Vec2 p3 = m_ps[i + 2];

		Vec2 d1 = p2 - p1;
		Vec2 d2 = p3 - p2;

		float32 a = Cross(d1, d2);
		float32 b = Dot(d1, d2);

		m_as[i] = atan2f(a, b);
	}

	m_gravity = def->gravity;
	m_damping = def->damping;
	m_k2 = def->k2;
	m_k3 = def->k3;
}

void Rope::Step(float32 h, int32 iterations)
{
	if (h == 0.0)
	{
		return;
	}

	float32 d = expf(- h * m_damping);

	for (int32 i = 0; i < m_count; ++i)
	{
		m_p0s[i] = m_ps[i];
		if (m_ims[i] > 0.0f)
		{
			m_vs[i] += h * m_gravity;
		}
		m_vs[i] *= d;
		m_ps[i] += h * m_vs[i];

	}

	for (int32 i = 0; i < iterations; ++i)
	{
		SolveC2();
		SolveC3();
		SolveC2();
	}

	float32 inv_h = 1.0f / h;
	for (int32 i = 0; i < m_count; ++i)
	{
		m_vs[i] = inv_h * (m_ps[i] - m_p0s[i]);
	}
}

void Rope::SolveC2()
{
	int32 count2 = m_count - 1;

	for (int32 i = 0; i < count2; ++i)
	{
		Vec2 p1 = m_ps[i];
		Vec2 p2 = m_ps[i + 1];

		Vec2 d = p2 - p1;
		float32 L = d.Normalize();

		float32 im1 = m_ims[i];
		float32 im2 = m_ims[i + 1];

		if (im1 + im2 == 0.0f)
		{
			continue;
		}

		float32 s1 = im1 / (im1 + im2);
		float32 s2 = im2 / (im1 + im2);

		p1 -= m_k2 * s1 * (m_Ls[i] - L) * d;
		p2 += m_k2 * s2 * (m_Ls[i] - L) * d;

		m_ps[i] = p1;
		m_ps[i + 1] = p2;
	}
}

void Rope::SetAngle(float32 angle)
{
	int32 count3 = m_count - 2;
	for (int32 i = 0; i < count3; ++i)
	{
		m_as[i] = angle;
	}
}

void Rope::SolveC3()
{
	int32 count3 = m_count - 2;

	for (int32 i = 0; i < count3; ++i)
	{
		Vec2 p1 = m_ps[i];
		Vec2 p2 = m_ps[i + 1];
		Vec2 p3 = m_ps[i + 2];

		float32 m1 = m_ims[i];
		float32 m2 = m_ims[i + 1];
		float32 m3 = m_ims[i + 2];

		Vec2 d1 = p2 - p1;
		Vec2 d2 = p3 - p2;

		float32 L1sqr = d1.LengthSquared();
		float32 L2sqr = d2.LengthSquared();

		if (L1sqr * L2sqr == 0.0f)
		{
			continue;
		}

		float32 a = Cross(d1, d2);
		float32 b = Dot(d1, d2);

		float32 angle = atan2f(a, b);

		Vec2 Jd1 = (-1.0f / L1sqr) * d1.Skew();
		Vec2 Jd2 = (1.0f / L2sqr) * d2.Skew();

		Vec2 J1 = -Jd1;
		Vec2 J2 = Jd1 - Jd2;
		Vec2 J3 = Jd2;

		float32 mass = m1 * Dot(J1, J1) + m2 * Dot(J2, J2) + m3 * Dot(J3, J3);
		if (mass == 0.0f)
		{
			continue;
		}

		mass = 1.0f / mass;

		float32 C = angle - m_as[i];

		while (C > pi)
		{
			angle -= 2 * pi;
			C = angle - m_as[i];
		}

		while (C < -pi)
		{
			angle += 2.0f * pi;
			C = angle - m_as[i];
		}

		float32 impulse = - m_k3 * mass * C;

		p1 += (m1 * impulse) * J1;
		p2 += (m2 * impulse) * J2;
		p3 += (m3 * impulse) * J3;

		m_ps[i] = p1;
		m_ps[i + 1] = p2;
		m_ps[i + 2] = p3;
	}
}

void Rope::Draw(b2::Draw* draw) const
{
	Color c(0.4f, 0.5f, 0.7f);

	for (int32 i = 0; i < m_count - 1; ++i)
	{
		draw->DrawSegment(m_ps[i], m_ps[i+1], c);
	}
}

} // namespace b2
