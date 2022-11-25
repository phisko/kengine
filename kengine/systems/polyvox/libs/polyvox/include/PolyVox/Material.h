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

#ifndef __PolyVox_Material_H__
#define __PolyVox_Material_H__

#include "Impl/PlatformDefinitions.h"

#include "DefaultIsQuadNeeded.h" //we'll specialise this function for this voxel type

namespace PolyVox
{
	///This class represents a voxel storing only a material.
	////////////////////////////////////////////////////////////////////////////////
	/// Note that this should probably just be considered an example of how to define
	/// a voxel type for the Marching Cubes algorithm. Advanced users are likely to
	/// define custom voxel types and possibly custom controllers.
	///
	/// \sa Density, MaterialDensityPair
	////////////////////////////////////////////////////////////////////////////////
	template <typename Type>
	class Material
	{
	public:
		Material() : m_uMaterial(0) {}
		Material(Type uMaterial) : m_uMaterial(uMaterial) {}

		bool operator==(const Material& rhs) const
		{
			return (m_uMaterial == rhs.m_uMaterial);
		};

		bool operator!=(const Material& rhs) const
		{
			return !(*this == rhs);
		}

		/// \return The current material value of the voxel
		Type getMaterial() const { return m_uMaterial; }
		/**
		 * Set the material value of the voxel
		 *
		 * \param uMaterial The material to set to
		 */
		void setMaterial(Type uMaterial) { m_uMaterial = uMaterial; }

	private:
		Type m_uMaterial;
	};

	typedef Material<uint8_t> Material8;
	typedef Material<uint16_t> Material16;

	template<typename Type>
	class DefaultIsQuadNeeded< Material<Type> >
	{
	public:
		bool operator()(Material<Type> back, Material<Type> front, Material<Type>& materialToUse)
		{
			if ((back.getMaterial() > 0) && (front.getMaterial() == 0))
			{
				materialToUse = back;
				return true;
			}
			else
			{
				return false;
			}
		}
	};
}

#endif //__PolyVox_Material_H__
