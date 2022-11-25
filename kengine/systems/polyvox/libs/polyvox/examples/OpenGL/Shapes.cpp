/*******************************************************************************
The MIT License (MIT)

Copyright (c) 2015 David Williams and Matthew Williams

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*******************************************************************************/

#include "Shapes.h"

#include "PolyVox/MaterialDensityPair.h"

using namespace PolyVox;

void createSphereInVolume(RawVolume<MaterialDensityPair88>& volData, float fRadius, uint8_t uValue)
{
	//This vector hold the position of the center of the volume
	Vector3DInt32 v3dVolCenter = (volData.getEnclosingRegion().getUpperCorner() - volData.getEnclosingRegion().getLowerCorner()) / static_cast<int32_t>(2);

	//This three-level for loop iterates over every voxel in the volume
	for (int z = 0; z < volData.getDepth(); z++)
	{
		for (int y = 0; y < volData.getHeight(); y++)
		{
			for (int x = 0; x < volData.getWidth(); x++)
			{
				//Store our current position as a vector...
				Vector3DInt32 v3dCurrentPos(x, y, z);
				//And compute how far the current position is from the center of the volume
				double fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

				//If the current voxel is less than 'radius' units from the center
				//then we make it solid, otherwise we make it empty space.
				if (fDistToCenter <= fRadius)
				{
					volData.setVoxel(x, y, z, MaterialDensityPair88(uValue, uValue > 0 ? MaterialDensityPair88::getMaxDensity() : MaterialDensityPair88::getMinDensity()));
				}
			}
		}
	}
}

void createCubeInVolume(RawVolume<MaterialDensityPair88>& volData, Vector3DInt32 lowerCorner, Vector3DInt32 upperCorner, uint8_t uValue)
{
	uint8_t maxDen = static_cast<uint8_t>(MaterialDensityPair88::getMaxDensity());
	uint8_t minDen = static_cast<uint8_t>(MaterialDensityPair88::getMinDensity());
	//This three-level for loop iterates over every voxel between the specified corners
	for (int z = lowerCorner.getZ(); z <= upperCorner.getZ(); z++)
	{
		for (int y = lowerCorner.getY(); y <= upperCorner.getY(); y++)
		{
			for (int x = lowerCorner.getX(); x <= upperCorner.getX(); x++)
			{
				volData.setVoxel(x, y, z, MaterialDensityPair88(uValue, uValue > 0 ? maxDen : minDen));
			}
		}
	}
}