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

#ifndef __PolyVox_MaterialDensityPair_H__
#define __PolyVox_MaterialDensityPair_H__

#include "DefaultIsQuadNeeded.h" //we'll specialise this function for this voxel type
#include "DefaultMarchingCubesController.h" //We'll specialise the controller contained in here

#include "Impl/PlatformDefinitions.h"

namespace PolyVox
{
	/// This class represents a voxel storing only a density.
	////////////////////////////////////////////////////////////////////////////////
	/// Note that this should probably just be considered an example of how to define
	/// a voxel type for the Marching Cubes algorithm. Advanced users are likely to
	/// define custom voxel types and possibly custom controllers.
	///
	/// \sa Density, Material
	////////////////////////////////////////////////////////////////////////////////
	template <typename Type, uint8_t NoOfMaterialBits, uint8_t NoOfDensityBits>
	class MaterialDensityPair
	{
	public:
		MaterialDensityPair() : m_uMaterial(0), m_uDensity(0) {}
		MaterialDensityPair(Type uMaterial, Type uDensity) : m_uMaterial(uMaterial), m_uDensity(uDensity) {}

		bool operator==(const MaterialDensityPair& rhs) const
		{
			return (m_uMaterial == rhs.m_uMaterial) && (m_uDensity == rhs.m_uDensity);
		};

		bool operator!=(const MaterialDensityPair& rhs) const
		{
			return !(*this == rhs);
		}

		MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>& operator+=(const MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>& rhs)
		{
			m_uDensity += rhs.m_uDensity;

			// What should we do with the material? Conceptually the idea of adding materials makes no sense, but for our
			// purposes we consider the 'sum' of two materials to just be the max. At least this way it is commutative.
			m_uMaterial = (std::max)(m_uMaterial, rhs.m_uMaterial);

			return *this;
		}

		MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>& operator/=(uint32_t rhs)
		{
			// There's nothing sensible we can do with the material, so this function only affects the density.
			m_uDensity /= rhs;
			return *this;
		}

		Type getDensity() const { return m_uDensity; }
		Type getMaterial() const { return m_uMaterial; }

		void setDensity(Type uDensity) { m_uDensity = uDensity; }
		void setMaterial(Type uMaterial) { m_uMaterial = uMaterial; }

		static Type getMaxDensity() { return (0x01 << NoOfDensityBits) - 1; }
		static Type getMinDensity() { return 0; }

	private:
		Type m_uMaterial : NoOfMaterialBits;
		Type m_uDensity : NoOfDensityBits;
	};

	template<typename Type, uint8_t NoOfMaterialBits, uint8_t NoOfDensityBits>
	class DefaultIsQuadNeeded< MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits> >
	{
	public:
		bool operator()(MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits> back, MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits> front, MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>& materialToUse)
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

	template <typename Type, uint8_t NoOfMaterialBits, uint8_t NoOfDensityBits>
	class DefaultMarchingCubesController< MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits> >
	{
	public:
		typedef Type DensityType;
		typedef Type MaterialType;

		DefaultMarchingCubesController(void)
		{
			// Default to a threshold value halfway between the min and max possible values.
			m_tThreshold = (MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>::getMinDensity() + MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits>::getMaxDensity()) / 2;
		}

		DefaultMarchingCubesController(DensityType tThreshold)
		{
			m_tThreshold = tThreshold;
		}

		DensityType convertToDensity(MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits> voxel)
		{
			return voxel.getDensity();
		}

		MaterialType convertToMaterial(MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits> voxel)
		{
			return voxel.getMaterial();
		}

		MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits> blendMaterials(MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits> a, MaterialDensityPair<Type, NoOfMaterialBits, NoOfDensityBits> b, float /*weight*/)
		{
			if (convertToDensity(a) > convertToDensity(b))
			{
				return a;
			}
			else
			{
				return b;
			}
		}

		DensityType getThreshold(void)
		{
			return m_tThreshold;
		}

		void setThreshold(DensityType tThreshold)
		{
			m_tThreshold = tThreshold;
		}

	private:
		DensityType m_tThreshold;
	};

	typedef MaterialDensityPair<uint8_t, 4, 4> MaterialDensityPair44;
	typedef MaterialDensityPair<uint16_t, 8, 8> MaterialDensityPair88;
}

#endif
