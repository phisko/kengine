/*******************************************************************************
* The MIT License (MIT)
*
* Copyright (c) 2015 Matthew Williams and David Williams
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

#include "TestVolumeSubclass.h"

#include "PolyVox/Array.h"

#include "PolyVox/BaseVolume.h"
#include "PolyVox/CubicSurfaceExtractor.h"
#include "PolyVox/Material.h"
#include "PolyVox/Vector.h"

#include <QtTest>

using namespace PolyVox;

template <typename VoxelType>
class VolumeSubclass : public BaseVolume<VoxelType>
{
public:
	//There seems to be some descrepency between Visual Studio and GCC about how the following class should be declared.
	//There is a work around (see also See http://goo.gl/qu1wn) given below which appears to work on VS2010 and GCC, but
	//which seems to cause internal compiler errors on VS2008 when building with the /Gm 'Enable Minimal Rebuild' compiler
	//option. For now it seems best to 'fix' it with the preprocessor insstead, but maybe the workaround can be reinstated
	//in the future
	//typedef BaseVolume<VoxelType> VolumeOfVoxelType; //Workaround for GCC/VS2010 differences. See http://goo.gl/qu1wn
	//class Sampler : public VolumeOfVoxelType::template Sampler< VolumeSubclass<VoxelType> >
#if defined(_MSC_VER)
	class Sampler : public BaseVolume<VoxelType>::Sampler< VolumeSubclass<VoxelType> > //This line works on VS2010
#else
	class Sampler : public BaseVolume<VoxelType>::template Sampler< VolumeSubclass<VoxelType> > //This line works on GCC
#endif
	{
	public:
		Sampler(VolumeSubclass<VoxelType>* volume)
			:BaseVolume<VoxelType>::template Sampler< VolumeSubclass<VoxelType> >(volume)
		{
				this->mVolume = volume;
		}
		//~Sampler();
	};

	/// Constructor for creating a fixed size volume.
	VolumeSubclass(const Region& regValid)
		:BaseVolume<VoxelType>()
		, mVolumeData(regValid.getWidthInVoxels(), regValid.getHeightInVoxels(), regValid.getDepthInVoxels())
	{
			//mVolumeData.resize(ArraySizes(this->getWidth())(this->getHeight())(this->getDepth()));
	}
	/// Destructor
	~VolumeSubclass() {};

	/// Gets a voxel at the position given by <tt>x,y,z</tt> coordinates
	VoxelType getVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos) const
	{
		if ((uXPos >= 0) && (uXPos < static_cast<int32_t>(mVolumeData.getDimension(0))) &&
			(uYPos >= 0) && (uYPos < static_cast<int32_t>(mVolumeData.getDimension(1))) &&
			(uZPos >= 0) && (uZPos < static_cast<int32_t>(mVolumeData.getDimension(2))))
		{
			return mVolumeData(uXPos, uYPos, uZPos);
		}
		else
		{
			return VoxelType();
		}
	}

	/// Gets a voxel at the position given by a 3D vector
	VoxelType getVoxel(const Vector3DInt32& v3dPos) const
	{
		return getVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ());
	}

	/// Sets the value used for voxels which are outside the volume
	void setBorderValue(const VoxelType& tBorder) { }
	/// Sets the voxel at the position given by <tt>x,y,z</tt> coordinates
	bool setVoxel(int32_t uXPos, int32_t uYPos, int32_t uZPos, VoxelType tValue)
	{
		if ((uXPos >= 0) && (uXPos < static_cast<int32_t>(mVolumeData.getDimension(0))) &&
			(uYPos >= 0) && (uYPos < static_cast<int32_t>(mVolumeData.getDimension(1))) &&
			(uZPos >= 0) && (uZPos < static_cast<int32_t>(mVolumeData.getDimension(2))))
		{
			mVolumeData(uXPos, uYPos, uZPos) = tValue;
			return true;
		}
		else
		{
			return false;
		}
	}
	/// Sets the voxel at the position given by a 3D vector
	bool setVoxel(const Vector3DInt32& v3dPos, VoxelType tValue) { return setVoxel(v3dPos.getX(), v3dPos.getY(), v3dPos.getZ(), tValue); }

	/// Calculates approximatly how many bytes of memory the volume is currently using.
	uint32_t calculateSizeInBytes(void) { return 0; }

private:
	Array<3, VoxelType> mVolumeData;
};

void TestVolumeSubclass::testExtractSurface()
{
	Region region(0, 0, 0, 16, 16, 16);
	VolumeSubclass<Material8> volumeSubclass(region);

	for (int32_t z = 0; z < region.getDepthInVoxels() / 2; z++)
	{
		for (int32_t y = 0; y < region.getHeightInVoxels(); y++)
		{
			for (int32_t x = 0; x < region.getWidthInVoxels(); x++)
			{
				Material8 mat(1);
				volumeSubclass.setVoxel(Vector3DInt32(x, y, z), mat);
			}
		}
	}

	auto result = extractCubicMesh(&volumeSubclass, region);

	QCOMPARE(result.getNoOfVertices(), static_cast<uint32_t>(8));
}

QTEST_MAIN(TestVolumeSubclass)
