********
Lighting
********
Lighting is an important part of creating a realistic scene, and fortunately most common lighting solutions can be easily applied to PolyVox meshes. In this document we describe how to implement dynamic lighting and ambient occlusion with PolyVox.

Dynamic Lighting
================
In general, any lighting solution for real-time 3D graphics should be directly applicable to PolyVox meshes.

Normal Calculation for Smooth Meshes
------------------------------------
When working with smooth voxel terrain meshes, PolyVox provides vertex normals as part of the extracted surface mesh. A common approach for computing these normals would be to compute normals for each face in the mesh, and then compute the vertex normals as a weighted average of the normals of the faces which share it. Actually this is not the approach used by PolyVox, as PolyVox instead computes the vertex normals directly from the underlying volume data.

More specifically, PolyVox is able to compute the *gradient* of the volume data at any given point using well established image processing methods. The normalised gradient value is used as the vertex normal and in general it is smoother than the value computed by averaging neighbouring faces.

Normal Calculation for Cubic Meshes
-----------------------------------
For cubic meshes PolyVox doesn't actually generate any vertex normals at all, and this is often a source of confusion for new users. The reason for this is that we wish to to perform per-face lighting rather than per-vertex lighting. Considering the case of a single cube, if we wanted to perform per-face lighting based on per-vertex normals then the normals cannot be shared between adjacent faces and so each vertex needs to be duplicated three times (one for each face which uses it). This means we would need 24 vertices to represent a cube which intuitively should only need eight vertices.

Therefore PolyVox does not generate per-vertex normals for cubic meshes, and as a result the cubic mesh's vertices are both smaller and less numerous. Of course, we still need a way to obtain normals for lighting calculations and so our suggestion is to compute the normals in a fragment program using the *derivative operations* which are provided by modern graphics hardware. 

The description here is rather oversimplified, but the idea behind these operations is that they can tell you how much a variable has changed between two adjacent pixels. If we use our fragment world space position as the input to these derivative operations then we can obtain two vectors which lie on the surface of our face. The cross product of these then gives us a vector which is perpendicular to both and which is therefore our normal.

Further information about the derivative operations can be found in the OpenGL/Direct3D API documentation, but the implementation in code is quite simple. Firstly you need to make sure that you have access to the fragment's world space position in your shader, which means you need to pass it through from the vertex shader. Then you can use the following code in your fragment shader:

.. sourcecode:: glsl

	vec3 worldNormal = cross(dFdy(inWorldPosition.xyz), dFdx(inWorldPosition.xyz));
	worldNormal = normalize(worldNormal);

.. note ::
	Depending on your graphics API and/or engine setup, you may actually need to flip the resulting normal.

Shadows
-------
To date we have only experimented with shadow maps as a solution to the real time shadowing problem and have found they work very well for both casting and receiving. The approach is essentially the same as for any other geometry and the usual approaches can be used for setting up the projection and for filtering the result. One PolyVox specific tip we can give is that you don't need to take account of materials when rendering into the shadow map as you always draw in black, so if you are splitting you geometry for material blending or for handling a large number of materials then you don't need to do this when rendering shadows. Using separate shadow geometry with all materials combined may decrease your batch count in this case.

The most widely used alternative to shadow maps is shadow volumes but we have not tested these with PolyVox. We do not expect these will provide a good solution because meshes usually require additional edge information to allow the shadow volume to be extruded from the silhouette and PolyVox does not provide this. Even if this edge information could be calculated, it would be invalidated each time the mesh changed which would make dynamic terrain more difficult.

Overall we would recommend you make use of shadow maps for dynamic shadows.

Ambient Occlusion
=================
This is an area in which we want to undertake more research in order to get effective ambient occlusion into PolyVox scenes. In the mean time SSAO has proved to be a popular solution.