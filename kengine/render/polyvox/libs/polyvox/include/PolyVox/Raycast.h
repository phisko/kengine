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

#ifndef __PolyVox_Raycast_H__
#define __PolyVox_Raycast_H__

#include "Vector.h"

namespace PolyVox
{
	namespace RaycastResults
	{
		/**
		 * The results of a raycast
		 */
		enum RaycastResult
		{
			Completed, ///< If the ray passed through the volume without being interupted
			Interupted ///< If the ray was interupted while travelling
		};
	}
	typedef RaycastResults::RaycastResult RaycastResult;

	/// OUT OF DATE SINCE UNCLASSING
	////////////////////////////////////////////////////////////////////////////////
	/// \file Raycast.h
	///
	/// The principle behind raycasting is to fire a 'ray' through the volume and determine
	/// what (if anything) that ray hits. This simple test can be used for the purpose of
	/// picking, visibility checks, lighting calculations, or numerous other applications.
	///
	/// A ray is a stright line in space define by a start point and a direction vector.
	/// The length of the direction vector represents the length of the ray. When you 
	/// execute a raycast it will iterate over each voxel which lies on the ray,
	/// starting from the defined start point. It will examine each voxel and terminate
	/// either when it encounters a solid voxel or when it reaches the end of the ray.
	///
	/// **Important Note:** These has been confusion in the past with people not realising
	/// that the length of the direction vector is important. Most graphics API can provide
	/// a camera position and view direction for picking purposes, but the view direction is
	/// usually normalised (i.e. of length one). If you use this view direction directly you
	/// will only iterate over a single voxel and won't find what you are looking for. Instead
	/// you must scale the direction vector so that it's length represents the maximum distance
	/// over which you want the ray to be cast.
	///
	/// Some further notes, the Raycast uses full 26-connectivity, which basically means it 
	/// will examine every voxel the ray touches, even if it just passes through the corner.
	/// Also, it peforms a simple binary test against a voxel's threshold, rather than making
	/// use of it's density. Therefore it will work best in conjunction with one of the 'cubic'
	/// surace extractors. It's behaviour with the Marching Cubes surface extractor has not
	/// been tested yet.
	///
	/// Note that we also have a pickVoxel() function which provides a slightly higher-level interface.
	////////////////////////////////////////////////////////////////////////////////

	template<typename VolumeType, typename Callback>
	RaycastResult raycastWithEndpoints(VolumeType* volData, const Vector3DFloat& v3dStart, const Vector3DFloat& v3dEnd, Callback& callback);

	template<typename VolumeType, typename Callback>
	RaycastResult raycastWithDirection(VolumeType* volData, const Vector3DFloat& v3dStart, const Vector3DFloat& v3dDirectionAndLength, Callback& callback);
}

#include "Raycast.inl"

#endif //__PolyVox_Raycast_H__
