***************
Level of Detail
***************
When the PolyVox surface extractors are applied to volume data the resulting mesh can contain a very high number of triangles. For large voxel worlds this can cause both performance and memory problems. The performance problems occur due the the load on the vertex shader which has to process a large number of vertices, and also due to the setup costs of a large number of tiny (possibly sub-pixel) triangles. The memory costs result simply from having a large amount of data which does not actually contribute to the visual appearance of the scene.

For these reasons it is desirable to reduce the triangle count of the meshes as far as possible, especially as meshes move away from the camera. This document describes the various approaches which are available within PolyVox to achieve this. Generally these approaches are different for cubic meshes vs smooth meshes and so we address these cases separately.

Cubic Meshes
============
A naive implementation of a cubic surface extractor would generate a mesh containing a quad for each voxel face which lies on the boundary between a solid and an empty voxel. Our extractCubicSurface() function is indeed capable of generating such a mesh, but it also provides the option to merge adjacent quads into a single quad subject to various conditions being satisfied (e.g. the faces must have the same material). This merging process drastically reduces the amount of geometry which must be drawn but does not modify the shape of the mesh. Because of these desirable properties such merging is performed by default, but it can be disabled if necessary.

To our knowledge the only drawback of performing this quad merging is that it can create T-junctions in the resulting mesh. T-junctions are an undesirable property of mesh geometry because they can cause tiny cracks (usually just seen as flickering pixels) to occur between quads. Whether it's a problem in practice depends on hardware precision (16/32 bit), distance from origin, number of transforms which are applied, and probably a number of other factors. We have yet to investigate.

We don't currently have a real solution to this problem. In Voxeliens the borders between voxels were darkened to simulate ambient occlusion and this had the desirable side effect of making any flickering pixels very hard to see. It's also possible that anti-aliasing strategies can reduce the problem, and storing vertex positions as integers may help as well. Lastly, it may be possible to construct some kind of post-process which would repair the image where it identifies single pixel discontinuities in the depth buffer.

For more information on this topic we suggest the following article series: `Meshing in Voxel Engines <https://blackflux.wordpress.com/2014/02/23/meshing-in-voxel-engines-part-1/>`_

Smooth Meshes
=============
Level of detail for smooth meshes is a lot more complex than for cubic ones, and we'll admit upfront that we do not currently have a good solution to this problem. None the less, we do have a couple of partial solutions which you might be able to use or adapt for your specific scenario.

Techniques for performing level of detail on smooth meshes basically fall into two categories. The first category involves reducing the resolution of the volume data and then running the surface extractor on the smaller volume. This naturally generates a lower detail mesh which must then be scaled up to match the other meshes in the scene. The second category involves generating the mesh at full detail and then using traditional mesh simplification techniques to reduces the number of triangles. Both techniques are explored in more detail below.

Volume Reduction
----------------
The VolumeResampler class can be used to copy volume data from a source region to a destination region, and it handles the resampling of the voxel values in the event that the source and destination regions are not the same size. This is exactly what we need for implementing level of detail and the principle is demonstrated by the SmoothLOD sample (see the documentation for the SmoothLOD sample for more information).

One of the problems with this approach is that the lower resolution mesh does not *exactly* line up with the higher resolution mesh, and this can cause cracks to be visible where the two meshes meet. The SmoothLOD sample attempts to avoid this problem by overlapping the meshes slightly but this may not be effective in all situations or from all viewpoints.

An alternative is the `Transvoxel algorithm <http://www.terathon.com/voxels/>`_ developed by Eric Lengyel. This essentially extends the original Marching Cubes lookup table with additional entries which handle seamless transitions between LOD levels, and it is a very promising solution to level of detail for voxel terrain. At this point in time we do not have an implementation of this algorithm.

However, in all volume reduction approaches there is some uncertainty about how materials should be handled. Creating a lower resolution volume means that several voxel values from the high resolution volume need to be combined into a single value. For density values this is straightforward as a simple average gives good results, but it is not clear how this extends to material identifiers. Averaging them doesn't make sense, and it is hard to imagine an approach which would not lead to visible artifacts as LOD levels change. Perhaps the visible effects can be reduced by blending between two LOD levels, but more investigation needs to be done here.

Mesh Simplification
-------------------
The other main approach is to generate the mesh at the full resolution and then reduce the number of triangles using a postprocessing step. This can draw on the large body of mesh simplification research and typically involves merging adjacent faces or collapsing vertices. When using this approach there are a couple of additional complications compared to the implementations which are normally seen.

The first additional complication is that the decimation algorithm needs to preserve material boundaries so that they don't move between LOD levels. When choosing whether a particular simplification can be made (i.e deciding if one vertex can be collapsed on to another or whether two faces can be merged) a metric is usually used to determine how much the simplification would affect the visual appearance of the mesh. When working with smooth voxel meshes this metric needs to also consider the material identifiers.

We also need to ensure that the metric preserves the geometric boundary of the mesh, so that no cracks are visible when a simplified mesh is placed next to an original one. Maintaining this geometric boundary can be difficult, as the straightforward approach of locking the edge vertices in place will tend to limit the amount of simplification which can be performed. Alternatively, cracks can be allowed to develop if they are later hidden through the use of 'skirts' around the resulting mesh.

PolyVox used to contain code for performing simplification of the smooth voxel meshes, but unfortunately it had significant performance and functionality issues and so it has been removed. We will instead investigate the use of external mesh simplification libraries and `OpenMesh <http://www.openmesh.org/>`_ may be a good candidate here.
