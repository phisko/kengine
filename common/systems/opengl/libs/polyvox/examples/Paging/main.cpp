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
#include "Perlin.h"

#include "PolyVox/MaterialDensityPair.h"
#include "PolyVox/CubicSurfaceExtractor.h"
#include "PolyVox/MarchingCubesSurfaceExtractor.h"
#include "PolyVox/Mesh.h"
#include "PolyVox/PagedVolume.h"

#include <QApplication>

// Use the PolyVox namespace
using namespace PolyVox;

/**
 * Generates data using Perlin noise.
 */
class PerlinNoisePager : public PolyVox::PagedVolume<MaterialDensityPair44>::Pager
{
public:
	/// Constructor
	PerlinNoisePager()
		:PagedVolume<MaterialDensityPair44>::Pager()
	{
	}

	/// Destructor
	virtual ~PerlinNoisePager() {};

	virtual void pageIn(const PolyVox::Region& region, PagedVolume<MaterialDensityPair44>::Chunk* pChunk)
	{
		Perlin perlin(2, 2, 1, 234);

		for (int x = region.getLowerX(); x <= region.getUpperX(); x++)
		{
			for (int y = region.getLowerY(); y <= region.getUpperY(); y++)
			{
				float perlinVal = perlin.Get(x / static_cast<float>(255 - 1), y / static_cast<float>(255 - 1));
				perlinVal += 1.0f;
				perlinVal *= 0.5f;
				perlinVal *= 255;
				for (int z = region.getLowerZ(); z <= region.getUpperZ(); z++)
				{
					MaterialDensityPair44 voxel;
					if (z < perlinVal)
					{
						const int xpos = 50;
						const int zpos = 100;
						if ((x - xpos)*(x - xpos) + (z - zpos)*(z - zpos) < 200)
						{
							// tunnel
							voxel.setMaterial(0);
							voxel.setDensity(MaterialDensityPair44::getMinDensity());
						}
						else
						{
							// solid
							voxel.setMaterial(245);
							voxel.setDensity(MaterialDensityPair44::getMaxDensity());
						}
					}
					else
					{
						voxel.setMaterial(0);
						voxel.setDensity(MaterialDensityPair44::getMinDensity());
					}

					// Voxel position within a chunk always start from zero. So if a chunk represents region (4, 8, 12) to (11, 19, 15)
					// then the valid chunk voxels are from (0, 0, 0) to (7, 11, 3). Hence we subtract the lower corner position of the
					// region from the volume space position in order to get the chunk space position.
					pChunk->setVoxel(x - region.getLowerX(), y - region.getLowerY(), z - region.getLowerZ(), voxel);
				}
			}
		}
	}

	virtual void pageOut(const PolyVox::Region& region, PagedVolume<MaterialDensityPair44>::Chunk* /*pChunk*/)
	{
		std::cout << "warning unloading region: " << region.getLowerCorner() << " -> " << region.getUpperCorner() << std::endl;
	}
};

class PagingExample : public PolyVoxExample
{
public:
	PagingExample(QWidget *parent)
		:PolyVoxExample(parent)
	{
	}

protected:
	void initializeExample() override
	{
		PerlinNoisePager* pager = new PerlinNoisePager();
		PagedVolume<MaterialDensityPair44> volData(pager, 8 * 1024 * 1024, 64);

		// Just some tests of memory usage, etc. 
		std::cout << "Memory usage: " << (volData.calculateSizeInBytes() / 1024.0 / 1024.0) << "MB" << std::endl;
		PolyVox::Region reg(Vector3DInt32(-255, 0, 0), Vector3DInt32(255, 255, 255));
		std::cout << "Prefetching region: " << reg.getLowerCorner() << " -> " << reg.getUpperCorner() << std::endl;
		volData.prefetch(reg);
		std::cout << "Memory usage: " << (volData.calculateSizeInBytes() / 1024.0 / 1024.0) << "MB" << std::endl;
		std::cout << "Flushing entire volume" << std::endl;
		volData.flushAll();
		std::cout << "Memory usage: " << (volData.calculateSizeInBytes() / 1024.0 / 1024.0) << "MB" << std::endl;

		// Extract the surface
		PolyVox::Region reg2(Vector3DInt32(0, 0, 0), Vector3DInt32(254, 254, 254));
		auto mesh = extractCubicMesh(&volData, reg2);
		std::cout << "#vertices: " << mesh.getNoOfVertices() << std::endl;

		auto decodedMesh = decodeMesh(mesh);

		// Pass the surface to the OpenGL window
		addMesh(decodedMesh);

		setCameraTransform(QVector3D(300.0f, 300.0f, 300.0f), -(PI / 4.0f), PI + (PI / 4.0f));
	}
};

int main(int argc, char *argv[])
{
	// Create and show the Qt OpenGL window
	QApplication app(argc, argv);
	PagingExample openGLWidget(0);
	openGLWidget.show();

	// Run the message pump.
	return app.exec();
}
