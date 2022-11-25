**************************
Frequently Asked Questions
**************************

Should I store my environment in a single big volume or break it down into several smaller ones?
------------------------------------------------------------------------------------------------
In most cases you should store you data in a single big volume, unless you are sure you understand the implications of doing otherwise.

Most algorithms in PolyVox operate on only a single volume (the exception to this are some of the image processing algorithms which use source and destination volumes, but even they use a *single* source and a *single* destination). The reason for this is that many algorithms require fast access to the neighbours of any given voxel. As an example, the surface extractors need to look at the neighbours of a voxel in order to determine whether triangles should be generated.

PolyVox volumes make it easy to access these neighbours, but the situation gets complex on the edge of a volume because the neighbouring voxels may not exist. It is possible to define a border value (which will be returned whenever you try to read a voxel outside the volume) but this doesn't handle all scenarios in the desired way. Often the most practical solution is to make the volume slightly larger than the data it needs to contain, and then avoid having your algorithms go right to the edge.

Having established that edge cases can be problematic, we can now see that storing your data as a set of adjacent volumes is undesirable because these edge cases then exist throughout the data set. This causes a lot of problems such as gaps between pieces of extracted terrain or discontinuities in the computed normals.

The usual reason why people attempt to break their terrain into separate volumes is so that they can perform some more advanced memory management for very big terrain, for example by only loading particular volumes into memory when they are within a certain distance from the camera. However, this kind of paging behaviour is already implemented by the PagedVolume class. The PagedVolume internally stores its data as a set of blocks, and does it in such a way that it is able to perform neighbourhood access across block boundaries. Whenever you find yourself trying to break terrain data into separate volumes you should probably use the PagedVolume instead.

Note that although you should only use a single volume for your data, it is still recommended that you split the mesh data into multiple pieces so that they can be culled against the view frustum, and so that you can update the smaller pieces more quickly when you need to. You can extract meshes from different parts of the volume by passing a Region to the surface extractor.

Lastly, note that there are exceptions to the 'one volume' rule. An example might be if you had a number of planets in space, in which case each planet could safely be a separate volume. These planets never touch, and so the artifacts which would occur on volume boundaries do not cause a problem.

Can I combine smooth meshes with cubic ones?
--------------------------------------------
We have never attempted to do this but in theory it should be possible. One option is simply to generate two meshes (one using extractMarchingCubesSurface() and the other using extractCubicSurface()) and render them on top of each other while allowing the depth buffer to resolve the intersections. Combining these two meshes into a single mesh is likely to be difficult as they use different vertex formats and have different texturing requirements (see the document on texture mapping).

An alternative possibility may be to create a new surface extractor based on the Surface Nets (link) algorithm. The idea here is that the mesh would start with a cubic shape, but as the net was stretched it would be smoothed. The degree of smoothing could be controlled by a voxel property which could allow the full range from cubic to smooth to exist in a single mesh. As mentioned above, this may mean that some extra work has to be put into a fragment shader which is capable of texturing this kind of mesh. Come by the forums of you want to discuss this further.