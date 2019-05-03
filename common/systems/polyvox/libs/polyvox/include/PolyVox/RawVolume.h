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

#ifndef __PolyVox_RawVolume_H__
#define __PolyVox_RawVolume_H__

#include "BaseVolume.h"
#include "Region.h"
#include "Vector.h"

#include <cstdlib> //For abort()
#include <limits>
#include <memory>
#include <stdexcept> //For invalid_argument

namespace PolyVox
{
	/**
	 * Simple volume implementation which stores data in a single large 3D array.
	 *
	 * This class is less memory-efficient than the PagedVolume, but it is the simplest possible
	 * volume implementation which makes it useful for debugging and getting started with PolyVox.
	 */
	template <typename VoxelType>
	class RawVolume : public BaseVolume<VoxelType>
	{
	public:
#ifndef SWIG
		//There seems to be some descrepency between Visual Studio and GCC about how the following class should be declared.
		//There is a work around (see also See http://goo.gl/qu1wn) given below which appears to work on VS2010 and GCC, but
		//which seems to cause internal compiler errors on VS2008 when building with the /Gm 'Enable Minimal Rebuild' compiler
		//option. For now it seems best to 'fix' it with the preprocessor insstead, but maybe the workaround can be reinstated
		//in the future
		//typedef Volume<VoxelType> VolumeOfVoxelType; //Workaround for GCC/VS2010 differences.
		//class Sampler : public VolumeOfVoxelType::template Sampler< RawVolume<VoxelType> >
#if defined(_MSC_VER)
		class Sampler : public BaseVolume<VoxelType>::Sampler< RawVolume<VoxelType> > //This line works on VS2010
#else
		class Sampler : public BaseVolume<VoxelType>::template Sampler< RawVolume<VoxelType> > //This line works on GCC
#endif
		{
		public:
			Sampler(RawVolume<VoxelType>* volume);
			~Sampler();

			inline VoxelType getVoxel(void) const;

			bool isCurrentPositionValid(void) const;

			void setPosition(const Vector3DInt32& v3dNewPos);
			void setPosition(int32_t xPos, int32_t yPos, int32_t zPos);
			inline bool setVoxel(VoxelType tValue);

			void movePositiveX(void);
			void movePositiveY(void);
			void movePositiveZ(void);

			void moveNegativeX(void);
			void moveNegativeY(void);
			void moveNegativeZ(void);

			inline VoxelType peekVoxel1nx1ny1nz(void) const;
			inline VoxelType peekVoxel1nx1ny0pz(void) const;
			inline VoxelType peekVoxel1nx1ny1pz(void) const;
			inline VoxelType peekVoxel1nx0py1nz(void) const;
			inline VoxelType peekVoxel1nx0py0pz(void) const;
			inline VoxelType peekVoxel1nx0py1pz(void) const;
			inline VoxelType peekVoxel1nx1py1nz(void) const;
			inline VoxelType peekVoxel1nx1py0pz(void) const;
			inline VoxelType peekVoxel1nx1py1pz(void) const;

			inline VoxelType peekVoxel0px1ny1nz(void) const;
			inline VoxelType peekVoxel0px1ny0pz(void) const;
			inline VoxelType peekVoxel0px1ny1pz(void) const;
			inline VoxelType peekVoxel0px0py1nz(void) const;
			inline VoxelType peekVoxel0px0py0pz(void) const;
			inline VoxelType peekVoxel0px0py1pz(void) const;
			inline VoxelType peekVoxel0px1py1nz(void) const;
			inline VoxelType peekVoxel0px1py0pz(void) const;
			inline VoxelType peekVoxel0px1py1pz(void) const;

			inline VoxelType peekVoxel1px1ny1nz(void) const;
			inline VoxelType peekVoxel1px1ny0pz(void) const;
			inline VoxelType peekVoxel1px1ny1pz(void) const;
			inline VoxelType peekVoxel1px0py1nz(void) const;
			inline VoxelType peekVoxel1px0py0pz(void) const;
			inline VoxelType peekVoxel1px0py1pz(void) const;
			inline VoxelType peekVoxel1px1py1nz(void) const;
			inline VoxelType peekVoxel1px1py0pz(void) const;
			inline VoxelType peekVoxel1px1py1pz(void) const;

		private:

			//Other current position information
			VoxelType* mCurrentVoxel;

			//Whether the current position is inside the volume
			//FIXME - Replace these with flags
			bool m_bIsCurrentPositionValidInX;
			bool m_bIsCurrentPositionValidInY;
			bool m_bIsCurrentPositionValidInZ;
		};
#endif // SWIG

	public:
		/// Constructor for creating a fixed size volume.
		RawVolume(const Region& regValid);

		/// Destructor
		~RawVolume();

		/// Gets the value used for voxels which are outside the volume
		VoxelType getBorderValue(void) const;
		/// Gets a Region representing the extents of the Volume.
		const Region& getEnclosingRegion(void) const;

		/// Gets the width of the volume in voxels.
		int32_t getWidth(void) const;
		/// Gets the height of the volume in voxels.
		int32_t getHeight(void) const;
		/// Gets the depth of the volume in voxels.
		int32_t getDepth(void) const;

		/// Gets a voxel at the position given by <tt>x,y,z</tt> coordinates
		VoxelType getVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos) const;
		/// Gets a voxel at the position given by a 3D vector
		VoxelType getVoxel(const Vector3DInt32& v3dPos) const;

		/// Sets the value used for voxels which are outside the volume
		void setBorderValue(const VoxelType& tBorder);
		/// Sets the voxel at the position given by <tt>x,y,z</tt> coordinates
		void setVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue);
		/// Sets the voxel at the position given by a 3D vector
		void setVoxel(const Vector3DInt32& v3dPos, VoxelType tValue);

		/// Calculates approximatly how many bytes of memory the volume is currently using.
		uint32_t calculateSizeInBytes(void);

	protected:
		/// Copy constructor
		RawVolume(const RawVolume& rhs);

		/// Assignment operator
		RawVolume& operator=(const RawVolume& rhs);

	private:
		void initialise(const Region& regValidRegion);

		//The size of the volume
		Region m_regValidRegion;

		//The border value
		VoxelType m_tBorderValue;

		//The voxel data
		VoxelType* m_pData;
	};
}

#include "RawVolume.inl"
#include "RawVolumeSampler.inl"

#endif //__PolyVox_RawVolume_H__
