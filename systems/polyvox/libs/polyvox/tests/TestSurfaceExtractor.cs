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

using System.Diagnostics;

public class test
{
	public static void Main()
	{
		Region r = new Region(new Vector3Dint32_t(0,0,0), new Vector3Dint32_t(31,31,31));
		SimpleVolumeuint8 vol = new SimpleVolumeuint8(r);
		//Set one single voxel to have a reasonably high density
		vol.setVoxelAt(new Vector3Dint32_t(5, 5, 5), 200);
		SurfaceMeshPositionMaterialNormal mesh = new SurfaceMeshPositionMaterialNormal();
		MarchingCubesSurfaceExtractorSimpleVolumeuint8 extractor = new MarchingCubesSurfaceExtractorSimpleVolumeuint8(vol, r, mesh);
		extractor.execute();
		
		Debug.Assert(mesh.getNoOfVertices() == 6);
		
		Vector3Dint32_t v1 = new Vector3Dint32_t(1,2,3);
		Vector3Dint32_t v2 = new Vector3Dint32_t(6,8,12);
		Vector3Dint32_t v3 = v1 + v2;
		
		Vector3Dint32_t v11 = new Vector3Dint32_t(1,2,3);
		
		Debug.Assert(v3.getX() == 7);
		Debug.Assert((v3*5).getX() == 35);
		Debug.Assert(v1.Equals(v11));
		Debug.Assert(v1 != v11);
		Debug.Assert(!v1.Equals(v2));
		Debug.Assert(!v1.Equals(null));

		System.Console.WriteLine("Success");
	}
}
