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

#ifndef __AmbientOcclusionCalculator_H__
#define __AmbientOcclusionCalculator_H__

#include "Impl/RandomUnitVectors.h"
#include "Impl/RandomVectors.h"

#include "Array.h"
#include "Region.h"
#include "Raycast.h"

#include <algorithm>

namespace PolyVox
{
	/**
	 * \file
	 *
	 * Ambient occlusion
	 */

	template<typename VolumeType, typename IsVoxelTransparentCallback>
	class AmbientOcclusionCalculatorRaycastCallback
	{
	public:
		AmbientOcclusionCalculatorRaycastCallback(IsVoxelTransparentCallback isVoxelTransparentCallback) : mIsVoxelTransparentCallback(isVoxelTransparentCallback)
		{
		}

		bool operator()(const typename VolumeType::Sampler& sampler)
		{
			auto sample = sampler.getVoxel();
			bool func = mIsVoxelTransparentCallback(sample);
			return func;
		}

		IsVoxelTransparentCallback mIsVoxelTransparentCallback;
	};

	// NOTE: The callback needs to be a functor not a function. I haven't been
	// able to work the required template magic to get functions working as well.
	// 
	// Matt: If you make the function take a "IsVoxelTransparentCallback&&" then
	// it will forward it on. Works for functors, functions and lambdas.
	// This will be 'perfect forwarding' using 'universal references'
	// This will require C++11 rvalue references which is why I haven't made the
	// change yet.

	/// Calculate the ambient occlusion for the volume
	template<typename VolumeType, typename IsVoxelTransparentCallback>
	void calculateAmbientOcclusion(VolumeType* volInput, Array<3, uint8_t>* arrayResult, const Region& region, float fRayLength, uint8_t uNoOfSamplesPerOutputElement, IsVoxelTransparentCallback isVoxelTransparentCallback);
}

#include "AmbientOcclusionCalculator.inl"

#endif //__AmbientOcclusionCalculator_H__
