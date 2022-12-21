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
	/**
	 * This function fills a 3D array with ambient occlusion values computed by raycasting through the volume.
	 * This approach to ambient occlusion is only appropriate for relatvely small volumes, otherwise it will 
	 * become very slow and consume a lot of memory. You will need to find a way to actually use the generated
	 * ambient occlusion data, which might mean uploading it the the GPU as a volume texture or sampling on
	 * the CPU using the vertex positions from your generated mesh.
	 *
	 * In practice we have not made much use of this implementation ourselves, so you may find it needs some
	 * optimizations or improvements to be useful. It is likely that there are actually better approaches to
	 * the ambient occlusion problem.
	 *
	 * \param volInput The volume to calculate the ambient occlusion for
	 * \param[out] arrayResult The output of the calculator
	 * \param region The region of the volume for which the occlusion should be calculated
	 * \param fRayLength The length for each test ray
	 * \param uNoOfSamplesPerOutputElement The number of samples to calculate the occlusion
	 * \param isVoxelTransparentCallback A callback which takes a \a VoxelType and returns a \a bool whether the voxel is transparent
	 */
	template<typename VolumeType, typename IsVoxelTransparentCallback>
	void calculateAmbientOcclusion(VolumeType* volInput, Array<3, uint8_t>* arrayResult, const Region& region, float fRayLength, uint8_t uNoOfSamplesPerOutputElement, IsVoxelTransparentCallback isVoxelTransparentCallback)
	{
		//Make sure that the size of the volume is an exact multiple of the size of the array.
		if (region.getWidthInVoxels() % arrayResult->getDimension(0) != 0)
		{
			POLYVOX_THROW(std::invalid_argument, "Volume width must be an exact multiple of array width.");
		}
		if (region.getHeightInVoxels() % arrayResult->getDimension(1) != 0)
		{
			POLYVOX_THROW(std::invalid_argument, "Volume width must be an exact multiple of array height.");
		}
		if (region.getDepthInVoxels() % arrayResult->getDimension(2) != 0)
		{
			POLYVOX_THROW(std::invalid_argument, "Volume width must be an exact multiple of array depth.");
		}

		uint16_t uRandomUnitVectorIndex = 0;
		uint16_t uRandomVectorIndex = 0;
		uint16_t uIndexIncreament;

		//Our initial indices. It doesn't matter exactly what we set here, but the code below makes 
		//sure they are different for different regions which helps reduce tiling patterns in the results.
		uRandomUnitVectorIndex += region.getLowerX() + region.getLowerY() + region.getLowerZ();
		uRandomVectorIndex += region.getLowerX() + region.getLowerY() + region.getLowerZ();

		//This value helps us jump around in the array a bit more, so the
		//nth 'random' value isn't always followed by the n+1th 'random' value.
		uIndexIncreament = 1;

		const int iRatioX = region.getWidthInVoxels() / arrayResult->getDimension(0);
		const int iRatioY = region.getHeightInVoxels() / arrayResult->getDimension(1);
		const int iRatioZ = region.getDepthInVoxels() / arrayResult->getDimension(2);

		const float fRatioX = iRatioX;
		const float fRatioY = iRatioY;
		const float fRatioZ = iRatioZ;
		const Vector3DFloat v3dRatio(fRatioX, fRatioY, fRatioZ);

		const float fHalfRatioX = fRatioX * 0.5f;
		const float fHalfRatioY = fRatioY * 0.5f;
		const float fHalfRatioZ = fRatioZ * 0.5f;
		const Vector3DFloat v3dHalfRatio(fHalfRatioX, fHalfRatioY, fHalfRatioZ);

		const Vector3DFloat v3dOffset(0.5f, 0.5f, 0.5f);

		//This loop iterates over the bottom-lower-left voxel in each of the cells in the output array
		for (uint16_t z = region.getLowerZ(); z <= region.getUpperZ(); z += iRatioZ)
		{
			for (uint16_t y = region.getLowerY(); y <= region.getUpperY(); y += iRatioY)
			{
				for (uint16_t x = region.getLowerX(); x <= region.getUpperX(); x += iRatioX)
				{
					//Compute a start position corresponding to 
					//the centre of the cell in the output array.
					Vector3DFloat v3dStart(x, y, z);
					v3dStart -= v3dOffset;
					v3dStart += v3dHalfRatio;

					//Keep track of how many rays did not hit anything
					uint8_t uVisibleDirections = 0;

					for (int ct = 0; ct < uNoOfSamplesPerOutputElement; ct++)
					{
						//We take a random vector with components going from -1 to 1 and scale it to go from -halfRatio to +halfRatio.
						//This jitter value moves our sample point from the centre of the array cell to somewhere else in the array cell
						Vector3DFloat v3dJitter = randomVectors[(uRandomVectorIndex += (++uIndexIncreament)) % 1019]; //Prime number helps avoid repetition on successive loops.
						v3dJitter *= v3dHalfRatio;
						const Vector3DFloat v3dRayStart = v3dStart + v3dJitter;

						Vector3DFloat v3dRayDirection = randomUnitVectors[(uRandomUnitVectorIndex += (++uIndexIncreament)) % 1021]; //Different prime number.
						v3dRayDirection *= fRayLength;

						AmbientOcclusionCalculatorRaycastCallback<VolumeType, IsVoxelTransparentCallback> ambientOcclusionCalculatorRaycastCallback(isVoxelTransparentCallback);
						RaycastResult result = raycastWithDirection(volInput, v3dRayStart, v3dRayDirection, ambientOcclusionCalculatorRaycastCallback);

						// Note - The performance of this could actually be improved it we exited as soon
						// as the ray left the volume. The raycast test has an example of how to do this.
						if (result == RaycastResults::Completed)
						{
							++uVisibleDirections;
						}
					}

					float fVisibility;
					if (uNoOfSamplesPerOutputElement == 0)
					{
						//The user might request zero samples (I've done this in the past while debugging - I don't want to
						//wait for ambient occlusion but I do want as valid result for rendering). Avoid the divide by zero.
						fVisibility = 1.0f;
					}
					else
					{
						fVisibility = static_cast<float>(uVisibleDirections) / static_cast<float>(uNoOfSamplesPerOutputElement);
						POLYVOX_ASSERT((fVisibility >= 0.0f) && (fVisibility <= 1.0f), "Visibility value out of range.");
					}

					(*arrayResult)(z / iRatioZ, y / iRatioY, x / iRatioX) = static_cast<uint8_t>(255.0f * fVisibility);
				}
			}
		}
	}
}
