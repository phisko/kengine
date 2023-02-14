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

#ifndef __PolyVox_Density_H__
#define __PolyVox_Density_H__

#include "DefaultMarchingCubesController.h" //We'll specialise the controller contained in here

#include "Impl/PlatformDefinitions.h"

#include <limits>

#undef min
#undef max

namespace PolyVox
{
	/// This class represents a voxel storing only a density.
	////////////////////////////////////////////////////////////////////////////////
	/// Note that this should probably just be considered an example of how to define
	/// a voxel type for the Marching Cubes algorithm. Advanced users are likely to
	/// define custom voxel types and possibly custom controllers.
	///
	/// \sa Material, MaterialDensityPair
	////////////////////////////////////////////////////////////////////////////////
	template <typename Type>
	class Density
	{
	public:
		/// Constructor
		Density() : m_uDensity(0) {}

		/// Copy constructor
		Density(Type uDensity) : m_uDensity(uDensity) {}

		// The LowPassFilter uses this to convert between normal and accumulated types.
		/// Copy constructor with cast
		template <typename CastType> explicit Density(const Density<CastType>& density)
		{
			m_uDensity = static_cast<Type>(density.getDensity());
		}

		bool operator==(const Density& rhs) const
		{
			return (m_uDensity == rhs.m_uDensity);
		};

		bool operator!=(const Density& rhs) const
		{
			return !(*this == rhs);
		}

		// For densities we can supply mathematical operators which behave in an intuitive way.
		// In particular the ability to add and subtract densities is important in order to
		// apply an averaging filter. The ability to divide by an integer is also needed for
		// this same purpose.
		Density<Type>& operator+=(const Density<Type>& rhs)
		{
			m_uDensity += rhs.m_uDensity;
			return *this;
		}

		Density<Type>& operator-=(const Density<Type>& rhs)
		{
			m_uDensity -= rhs.m_uDensity;
			return *this;
		}

		Density<Type>& operator/=(uint32_t rhs)
		{
			m_uDensity /= rhs;
			return *this;
		}

		/// \return The current density of the voxel
		Type getDensity() const { return m_uDensity; }
		/**
		 * Set the density of the voxel
		 *
		 * \param uDensity The density to set to
		 */
		void setDensity(Type uDensity) { m_uDensity = uDensity; }

		/// \return The maximum allowed density of the voxel
		static Type getMaxDensity() { return (std::numeric_limits<Type>::max)(); }
		/// \return The minimum allowed density of the voxel
		static Type getMinDensity() { return (std::numeric_limits<Type>::min)(); }

	private:
		Type m_uDensity;
	};

	template <typename Type>
	Density<Type> operator+(const Density<Type>& lhs, const Density<Type>& rhs)
	{
		Density<Type> result = lhs;
		result += rhs;
		return result;
	}

	template <typename Type>
	Density<Type> operator-(const Density<Type>& lhs, const Density<Type>& rhs)
	{
		Density<Type> result = lhs;
		result -= rhs;
		return result;
	}

	template <typename Type>
	Density<Type> operator/(const Density<Type>& lhs, uint32_t rhs)
	{
		Density<Type> result = lhs;
		result /= rhs;
		return result;
	}

	// These are the predefined density types. The 8-bit types are sufficient for many purposes (including
	// most games) but 16-bit and float types do have uses particularly in medical/scientific visualisation.
	typedef Density<uint8_t> Density8;
	typedef Density<uint16_t> Density16;
	typedef Density<uint32_t> Density32;
	typedef Density<float> DensityFloat;

	/**
	 * This is a specialisation of DefaultMarchingCubesController for the Density voxel type
	 */
	template <typename Type>
	class DefaultMarchingCubesController< Density<Type> >
	{
	public:
		typedef Type DensityType;
		typedef float MaterialType;

		DefaultMarchingCubesController(void)
		{
			// Default to a threshold value halfway between the min and max possible values.
			m_tThreshold = (Density<Type>::getMinDensity() + Density<Type>::getMaxDensity()) / 2;
		}

		DefaultMarchingCubesController(DensityType tThreshold)
		{
			m_tThreshold = tThreshold;
		}

		DensityType convertToDensity(Density<Type> voxel)
		{
			return voxel.getDensity();
		}

		MaterialType convertToMaterial(Density<Type> /*voxel*/)
		{
			return 1;
		}

		MaterialType blendMaterials(Density<Type> /*a*/, Density<Type> /*b*/, float /*weight*/)
		{
			return 1;
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
}

#endif //__PolyVox_Density_H__
