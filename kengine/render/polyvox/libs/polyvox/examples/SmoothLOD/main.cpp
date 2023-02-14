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

#include "PolyVoxExample.h"

#include "PolyVox/Density.h"
#include "PolyVox/MarchingCubesSurfaceExtractor.h"
#include "PolyVox/Mesh.h"
#include "PolyVox/RawVolume.h"
#include "PolyVox/VolumeResampler.h"

#include <QApplication>

//Use the PolyVox namespace
using namespace PolyVox;

void createSphereInVolume(RawVolume<uint8_t>& volData, float fRadius)
{
	//This vector hold the position of the center of the volume
	Vector3DFloat v3dVolCenter(volData.getWidth() / 2, volData.getHeight() / 2, volData.getDepth() / 2);

	//This three-level for loop iterates over every voxel in the volume
	for (int z = 0; z < volData.getDepth(); z++)
	{
		for (int y = 0; y < volData.getHeight(); y++)
		{
			for (int x = 0; x < volData.getWidth(); x++)
			{
				//Store our current position as a vector...
				Vector3DFloat v3dCurrentPos(x, y, z);
				//And compute how far the current position is from the center of the volume
				float fDistToCenter = (v3dCurrentPos - v3dVolCenter).length();

				if (fDistToCenter <= fRadius)
				{
					//Our new density value
					uint8_t uDensity = std::numeric_limits<uint8_t>::max();

					//Wrte the voxel value into the volume	
					volData.setVoxel(x, y, z, uDensity);
				}

				//144 in the middle, (144 - 32) at the edges. Threshold of 128 is between these
				//volData.setVoxel(x, y, z, 144 - fDistToCenter);
			}
		}
	}
}

class SmoothLODExample : public PolyVoxExample
{
public:
	SmoothLODExample(QWidget *parent)
		:PolyVoxExample(parent)
	{
	}

protected:
	void initializeExample() override
	{
		//Create an empty volume and then place a sphere in it
		RawVolume<uint8_t> volData(PolyVox::Region(Vector3DInt32(0, 0, 0), Vector3DInt32(63, 63, 63)));
		createSphereInVolume(volData, 28);

		//Smooth the data - should reimplement this using LowPassFilter
		//smoothRegion<PagedVolume, Density8>(volData, volData.getEnclosingRegion());
		//smoothRegion<PagedVolume, Density8>(volData, volData.getEnclosingRegion());
		//smoothRegion<PagedVolume, Density8>(volData, volData.getEnclosingRegion());

		RawVolume<uint8_t> volDataLowLOD(PolyVox::Region(Vector3DInt32(0, 0, 0), Vector3DInt32(15, 31, 31)));

		VolumeResampler< RawVolume<uint8_t>, RawVolume<uint8_t> > volumeResampler(&volData, PolyVox::Region(Vector3DInt32(0, 0, 0), Vector3DInt32(31, 63, 63)), &volDataLowLOD, volDataLowLOD.getEnclosingRegion());
		volumeResampler.execute();

		//Extract the surface
		auto meshLowLOD = extractMarchingCubesMesh(&volDataLowLOD, volDataLowLOD.getEnclosingRegion());
		// The returned mesh needs to be decoded to be appropriate for GPU rendering.
		auto decodedMeshLowLOD = decodeMesh(meshLowLOD);

		//Extract the surface
		auto meshHighLOD = extractMarchingCubesMesh(&volData, PolyVox::Region(Vector3DInt32(30, 0, 0), Vector3DInt32(63, 63, 63)));
		// The returned mesh needs to be decoded to be appropriate for GPU rendering.
		auto decodedMeshHighLOD = decodeMesh(meshHighLOD);

		//Pass the surface to the OpenGL window
		addMesh(decodedMeshHighLOD, Vector3DInt32(30, 0, 0));
		addMesh(decodedMeshLowLOD, Vector3DInt32(0, 0, 0), 63.0f / 31.0f);

		setCameraTransform(QVector3D(100.0f, 100.0f, 100.0f), -(PI / 4.0f), PI + (PI / 4.0f));
	}
};

int main(int argc, char *argv[])
{
	//Create and show the Qt OpenGL window
	QApplication app(argc, argv);
	SmoothLODExample openGLWidget(0);
	openGLWidget.show();

	//Run the message pump.
	return app.exec();
}
