/*******************************************************************************
* The MIT License (MIT)
*
* Copyright (c) 2015 David Williams and Matthew Williams
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*******************************************************************************/

namespace PolyVox
{
	template <typename IteratorType>
	void IteratorController<IteratorType>::reset(void)
	{
		m_Iter->setPosition(m_regValid.getLowerCorner());
	}

	template <typename IteratorType>
	bool IteratorController<IteratorType>::moveForward(void)
	{
		Vector3DInt32 v3dInitialPosition(m_Iter->getPosition().getX(), m_Iter->getPosition().getY(), m_Iter->getPosition().getZ());

		if (v3dInitialPosition.getX() < m_regValid.getUpperX())
		{
			m_Iter->movePositiveX();
			return true;
		}

		v3dInitialPosition.setX(m_regValid.getLowerX());

		if (v3dInitialPosition.getY() < m_regValid.getUpperY())
		{
			v3dInitialPosition.setY(v3dInitialPosition.getY() + 1);
			m_Iter->setPosition(v3dInitialPosition);
			return true;
		}

		v3dInitialPosition.setY(m_regValid.getLowerY());

		if (v3dInitialPosition.getZ() < m_regValid.getUpperZ())
		{
			v3dInitialPosition.setZ(v3dInitialPosition.getZ() + 1);
			m_Iter->setPosition(v3dInitialPosition);
			return true;
		}

		return false;
	}
}
