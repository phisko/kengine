# -*- coding: utf-8 -*-
################################################################################
# The MIT License (MIT)
#
# Copyright (c) 2015 Matthew Williams and David Williams
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
################################################################################

import sys
sys.path.append("../library/bindings/")

import unittest

class TestSurfaceExtractor(unittest.TestCase):
	def setUp(self):
		import PolyVoxCore
		
		#Create a small volume
		r = PolyVoxCore.Region(PolyVoxCore.Vector3Dint32_t(0,0,0), PolyVoxCore.Vector3Dint32_t(31,31,31))
		vol = PolyVoxCore.SimpleVolumeuint8(r)
		#Set one single voxel to have a reasonably high density
		vol.setVoxelAt(PolyVoxCore.Vector3Dint32_t(5, 5, 5), 200)
		self.mesh = PolyVoxCore.SurfaceMeshPositionMaterialNormal()
		extractor = PolyVoxCore.MarchingCubesSurfaceExtractorSimpleVolumeuint8(vol, r, self.mesh)
		extractor.execute()
	
	def test_num_vertices(self):
		self.assertEqual(self.mesh.getNoOfVertices(), 6)

if __name__ == '__main__':
	unittest.main()
