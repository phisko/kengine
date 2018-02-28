/*
* Copyright (c) 2011 Erin Catto http://www.box2d.org
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

#ifndef B2_ROPE_HPP
#define B2_ROPE_HPP

#include <Box2D/Common/Math.hpp>

namespace b2
{

class Draw;

/// 
struct RopeDef
{
	RopeDef()
	{
		vertices = NULL;
		count = 0;
		masses = NULL;
		gravity.SetZero();
		damping = 0.1f;
		k2 = 0.9f;
		k3 = 0.1f;
	}

	///
	Vec2* vertices;

	///
	int32 count;

	///
	float32* masses;

	///
	Vec2 gravity;

	///
	float32 damping;

	/// Stretching stiffness
	float32 k2;

	/// Bending stiffness. Values above 0.5 can make the simulation blow up.
	float32 k3;
};

/// 
class Rope
{
public:
	Rope();
	~Rope();

	///
	void Initialize(const RopeDef* def);

	///
	void Step(float32 timeStep, int32 iterations);

	///
	int32 GetVertexCount() const
	{
		return m_count;
	}

	///
	const Vec2* GetVertices() const
	{
		return m_ps;
	}

	///
	void Draw(Draw* draw) const;

	///
	void SetAngle(float32 angle);

private:

	void SolveC2();
	void SolveC3();

	int32 m_count;
	Vec2* m_ps;
	Vec2* m_p0s;
	Vec2* m_vs;

	float32* m_ims;

	float32* m_Ls;
	float32* m_as;

	Vec2 m_gravity;
	float32 m_damping;

	float32 m_k2;
	float32 m_k3;
};

} // namespace b2

#endif // B2_ROPE_HPP
