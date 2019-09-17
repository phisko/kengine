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

#include "PolyVox/CubicSurfaceExtractor.h"
#include "PolyVox/MarchingCubesSurfaceExtractor.h"
#include "PolyVox/Mesh.h"
#include "PolyVox/RawVolume.h"

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

				uint8_t uVoxelValue = 0;

				//If the current voxel is less than 'radius' units from the center then we make it solid.
				if (fDistToCenter <= fRadius)
				{
					//Our new voxel value
					uVoxelValue = 255;
				}

				//Wrte the voxel value into the volume	
				volData.setVoxel(x, y, z, uVoxelValue);
			}
		}
	}
}

class BasicExample : public PolyVoxExample
{
public:
	BasicExample(QWidget *parent)
		:PolyVoxExample(parent)
	{
	}

protected:
	void initializeExample() override
	{
		// Create an empty volume and then place a sphere in it
		RawVolume<uint8_t> volData(PolyVox::Region(Vector3DInt32(0, 0, 0), Vector3DInt32(63, 63, 63)));
		createSphereInVolume(volData, 30);

		// Extract the surface for the specified region of the volume. Uncomment the line for the kind of surface extraction you want to see.
		auto mesh = extractCubicMesh(&volData, volData.getEnclosingRegion());
		//auto mesh = extractMarchingCubesMesh(&volData, volData.getEnclosingRegion());

		// The surface extractor outputs the mesh in an efficient compressed format which is not directly suitable for rendering. The easiest approach is to 
		// decode this on the CPU as shown below, though more advanced applications can upload the compressed mesh to the GPU and decompress in shader code.
		auto decodedMesh = decodeMesh(mesh);

		//Pass the surface to the OpenGL window
		addMesh(decodedMesh);

		setCameraTransform(QVector3D(100.0f, 100.0f, 100.0f), -(PI / 4.0f), PI + (PI / 4.0f));
	}
};

int main(int argc, char *argv[])
{
	//Create and show the Qt OpenGL window
	QApplication app(argc, argv);
	BasicExample openGLWidget(0);
	openGLWidget.show();

	//Run the message pump.
	return app.exec();
}