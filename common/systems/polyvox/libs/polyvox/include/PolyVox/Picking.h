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

#ifndef __PolyVox_Picking_H__
#define __PolyVox_Picking_H__

#include "Vector.h"

namespace PolyVox
{
	/**
	 * A structure containing the information about a picking operation
	 */
	struct PickResult
	{
		PickResult() : didHit(false) {}
		bool didHit; ///< Did the picking operation hit anything
		Vector3DInt32 hitVoxel; ///< The location of the solid voxel it hit
		Vector3DInt32 previousVoxel; ///< The location of the voxel before the one it hit
	};

	/// Pick the first solid voxel along a vector
	template<typename VolumeType>
	PickResult pickVoxel(VolumeType* volData, const Vector3DFloat& v3dStart, const Vector3DFloat& v3dDirectionAndLength, const typename VolumeType::VoxelType& emptyVoxelExample);
}

#include "Picking.inl"

#endif //__PolyVox_Picking_H__
