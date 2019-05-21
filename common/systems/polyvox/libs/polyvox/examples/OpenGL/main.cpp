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

#include "PolyVox/FilePager.h"
#include "PolyVox/MarchingCubesSurfaceExtractor.h"
#include "PolyVox/MaterialDensityPair.h"
#include "PolyVox/LowPassFilter.h"
#include "PolyVox/RawVolume.h"
#include "PolyVox/Mesh.h"
#include "PolyVox/Impl/Utility.h"

#include "Shapes.h"

#include "PolyVoxExample.h"

#ifdef WIN32
#include <windows.h>   // Standard Header For Most Programs
#endif

#include <QApplication>
#include <QTime>

//Some namespaces we need
using namespace std;
using namespace PolyVox;
using namespace std;

const int32_t g_uVolumeSideLength = 128;

class OpenGLExample : public PolyVoxExample
{
public:
	OpenGLExample(QWidget *parent)
		:PolyVoxExample(parent)
	{
	}

protected:
	void initializeExample() override
	{
		RawVolume<MaterialDensityPair88> volData(PolyVox::Region(Vector3DInt32(0, 0, 0), Vector3DInt32(g_uVolumeSideLength - 1, g_uVolumeSideLength - 1, g_uVolumeSideLength - 1)));

		//Make our volume contain a sphere in the center.
		int32_t minPos = 0;
		int32_t midPos = g_uVolumeSideLength / 2;
		int32_t maxPos = g_uVolumeSideLength - 1;

		cout << "Creating sphere 1" << std::endl;
		createSphereInVolume(volData, 60.0f, 5);
		cout << "Creating sphere 2" << std::endl;
		createSphereInVolume(volData, 50.0f, 4);
		cout << "Creating sphere 3" << std::endl;
		createSphereInVolume(volData, 40.0f, 3);
		cout << "Creating sphere 4" << std::endl;
		createSphereInVolume(volData, 30.0f, 2);
		cout << "Creating sphere 5" << std::endl;
		createSphereInVolume(volData, 20.0f, 1);

		cout << "Creating cubes" << std::endl;
		createCubeInVolume(volData, Vector3DInt32(minPos, minPos, midPos + 1), Vector3DInt32(midPos - 1, midPos - 1, maxPos), 0);
		createCubeInVolume(volData, Vector3DInt32(midPos + 1, midPos + 1, midPos + 1), Vector3DInt32(maxPos, maxPos, maxPos), 0);
		createCubeInVolume(volData, Vector3DInt32(minPos, midPos + 1, minPos), Vector3DInt32(midPos - 1, maxPos, midPos - 1), 0);
		createCubeInVolume(volData, Vector3DInt32(midPos + 1, minPos, minPos), Vector3DInt32(maxPos, midPos - 1, midPos - 1), 0);

		createCubeInVolume(volData, Vector3DInt32(1, midPos - 10, midPos - 10), Vector3DInt32(maxPos - 1, midPos + 10, midPos + 10), MaterialDensityPair44::getMaxDensity());
		createCubeInVolume(volData, Vector3DInt32(midPos - 10, 1, midPos - 10), Vector3DInt32(midPos + 10, maxPos - 1, midPos + 10), MaterialDensityPair44::getMaxDensity());
		createCubeInVolume(volData, Vector3DInt32(midPos - 10, midPos - 10, 1), Vector3DInt32(midPos + 10, midPos + 10, maxPos - 1), MaterialDensityPair44::getMaxDensity());

		QSharedPointer<QGLShaderProgram> shader(new QGLShaderProgram);

		if (!shader->addShaderFromSourceFile(QGLShader::Vertex, ":/openglexample.vert"))
		{
			std::cerr << shader->log().toStdString() << std::endl;
			exit(EXIT_FAILURE);
		}

		if (!shader->addShaderFromSourceFile(QGLShader::Fragment, ":/openglexample.frag"))
		{
			std::cerr << shader->log().toStdString() << std::endl;
			exit(EXIT_FAILURE);
		}

		setShader(shader);

		QTime time;
		time.start();
		//openGLWidget.setVolume(&volData);
		cout << endl << "Time taken = " << time.elapsed() / 1000.0f << "s" << endl << endl;

		const int32_t extractedRegionSize = 32;
		int meshCounter = 0;

		for (int32_t z = 0; z < volData.getDepth(); z += extractedRegionSize)
		{
			for (int32_t y = 0; y < volData.getHeight(); y += extractedRegionSize)
			{
				for (int32_t x = 0; x < volData.getWidth(); x += extractedRegionSize)
				{
					// Specify the region to extract based on a starting position and the desired region sze.
					PolyVox::Region regToExtract(x, y, z, x + extractedRegionSize, y + extractedRegionSize, z + extractedRegionSize);

					// If you uncomment this line you will be able to see that the volume is rendered as multiple seperate meshes.
					//regToExtract.shrink(1);

					// Perform the extraction for this region of the volume
					auto mesh = extractMarchingCubesMesh(&volData, regToExtract);

					// The returned mesh needs to be decoded to be appropriate for GPU rendering.
					auto decodedMesh = decodeMesh(mesh);

					// Pass the surface to the OpenGL window. Note that we are also passing an offset in this multi-mesh example. This is because
					// the surface extractors return a mesh with 'local space' positions to reduce storage requirements and precision problems.
					addMesh(decodedMesh, decodedMesh.getOffset());

					meshCounter++;
				}
			}
		}

		cout << "Rendering volume as " << meshCounter << " seperate meshes" << endl;

		setCameraTransform(QVector3D(150.0f, 150.0f, 150.0f), -(PI / 4.0f), PI + (PI / 4.0f));
	}
};

int main(int argc, char *argv[])
{
	//Create and show the Qt OpenGL window
	QApplication app(argc, argv);
	OpenGLExample openGLWidget(0);
	openGLWidget.show();

	//Run the message pump.
	return app.exec();
}
