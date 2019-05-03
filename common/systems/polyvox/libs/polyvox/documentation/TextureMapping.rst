***************
Texture Mapping
***************
The PolyVox library is only concerned with operations on volume data (such as extracting a mesh from from a volume) and deliberately avoids the issue of rendering any resulting polygon meshes. This means PolyVox is not tied to any particular graphics API or rendering engine, and makes it much easier to integrate PolyVox with existing technology, because in general a PolyVox mesh can be treated the same as any other mesh. However, the texturing of a PolyVox mesh is usually handled a little differently, and so the purpose of this document is to provide some ideas about where to start with this process.

This document is aimed at readers in one of two positions:

1. You are trying to texture 'Minecraft-style' terrain with cubic blocks and a number of different materials.
2. You are trying to texture smooth terrain produced by the Marching Cubes (or similar) algorithm.

These are certainly not the limit of PolyVox, and you can choose much more advanced texturing approaches if you wish. For example, in the past we have texture mapped a voxel Earth from a cube map and used an animated *procedural* texture (based on Perlin noise) for the magma at the center of the Earth. However, if you are aiming for such advanced techniques then we assume you understand the basics in this document and have enough knowledge to expand the ideas yourself. But do feel free to drop by and ask questions on our forum.

Traditionally meshes are textured by providing a pair of UV texture coordinates for each vertex, and these UV coordinates determine which parts of a texture maps to each vertex. The process of texturing PolyVox meshes is more complex for a couple of reasons:

1. PolyVox does not provide UV coordinates for each vertex.
2. Voxel terrain (particularly Minecraft-style) often involves many more textures than the GPU can read at a time.

By reading this document you should learn how to work around the above problems, though you will almost certainly need to follow provided links and do some further reading as we have only summarised the key ideas here.

Mapping textures to mesh geometry
=================================
The lack of UV coordinates means some lateral thinking is required in order to apply texture maps to meshes. But before we get to that, we will first try to explain the rational behind PolyVox not providing UV coordinates in the first place. This rational is different for the smooth voxel meshes vs the cubic voxel meshes.

Rational
--------
The problem with texturing smooth voxel meshes is that the geometry can get very complex and it is not clear how the mapping between mesh geometry and a texture should be performed. In a traditional heightmap-based terrain this relationship is obvious as the texture map and heightmap simply line up directly. But for more complex shapes some form of 'UV unwrapping' is usually performed to define this relationship. This is usually done by an artist with the help of a 3D modeling package and so is a semi-automatic process, but it is time consuming and driven by the artists idea of what looks right for their particular scene. Even though fully automatic UV unwrapping is possible it is usually prohibitively slow.

Even if such an unwrapping were possible in a reasonable time frame, the next problem is that it would be invalidated as soon as the mesh changed. Enabling dynamic manipulation is one of the appealing factors of voxel terrain, and if this use case were discarded then the user may as well just model their terrain in an existing 3D modelling package and texture there. For these reasons we do not attempt to generate UV coordinates for smooth voxel meshes.

The rational in the cubic case is almost the opposite. For Minecraft style terrain you want to simply line up an instance of a texture with each face of a cube, and generating the texture coordinates for this is very easy. In fact it's so easy that there's no point in doing it - the logic can instead be implemented in a shader which in turn allows the amount of data in each vertex to be reduced.

Triplanar Texturing
-------------------
The most common approach to texture mapping smooth voxel terrain is to use *triplanar texturing*. The basic idea is to project a texture along all three main axes and blend between the three texture samples according to the surface normal. As an example, suppose that we wish to write a fragment shader to apply a single texture to our terrain, and assume that we have access to both the world space position of the fragment and also its normalised surface normal. Also, note that your textures should be set to wrap because the world space position will quickly go outside the bounds of 0.0-1.0. The world space position will need to have been passed through from earlier in the pipeline while the normal can be computed using one of the approaches in the :doc:`lighting <Lighting>` document. The shader code would then look something like this [footnote: code is untested as is simplified compared to real world code. hopefully it compiles, but if not it should still give you an idea of how it works]:

.. sourcecode:: glsl

	// Take the three texture samples
	vec4 sampleX = texture2d(inputTexture, worldSpacePos.yz); // Project along x axis
	vec4 sampleY = texture2d(inputTexture, worldSpacePos.xz); // Project along y axis
	vec4 sampleZ = texture2d(inputTexture, worldSpacePos.xy); // Project along z axis

	// Blend the samples according to the normal
	vec4 blendedColour = sampleX * normal.x + sampleY * normal.y + sampleZ * normal.z; 

Note that this approach will lead to the texture repeating once every world unit, and so in practice you may wish to scale the world space positions to make the texture appear the desired size. Also this technique can be extended to work with normal mapping though we won't go into the details here.

This idea of triplanar texturing can be applied to the cubic meshes as well, and in some ways it can be considered to be even simpler. With cubic meshes the normal always points exactly along one of the main axes, and so it is not necessary to sample the texture three times nor to blend the results. Instead you can use conditional branching in the fragment shader to determine which pair of values out of {x,y,z} should be used as the texture coordinates. Something like:

.. sourcecode:: glsl

	vec4 sample = vec4(0, 0, 0, 0); // We'll fill this in below
	// Assume the normal is normalised.
	if(normal.x > 0.9) // x must be one while y and z are zero
	{
		//Project onto yz plane
		sample = texture2D(inputTexture, worldSpacePos.yz);
	}
	// Now similar logic for the other two axes.
	.
	.
	.

You might also choose to sample a different texture for each of the axes, in order to apply a different texture to each face of your cube. If so, you probably want to pack your different face textures together using an approach similar to those described later in this document for multiple material textures. Another (untested) idea would be to use the normal to select a face on a 1x1x1 cubemap, and have the cubemap face contain an index value for addressing the correct face texture. This could bypass the conditional logic above.

Using the material identifier
-----------------------------
So far we have assumed that only a single material is being used for the entire voxel world, but this is seldom the case. It is common to associate a particular material with each voxel so that it can represent rock, wood, sand or any other type of material as required. The usual approach is to store a simple integer identifier with each voxel, and then map this identifier to material properties within your application.

Both extractCubicSurface() and the extractMarchingCubesSurface() include a 'data' member in their vertices, and this is copied/interpolated directly from the corresponding voxels. Therefore you can store a material identifier as part of your voxel type, and then pass this through to your shader as a vertex attribute. You can then use this to affect the way the surface is rendered.

The following code snippet assumes that you have passed a material identifier to your shaders and that you can access it in the fragment shader. It then chooses which colour to draw the polygon based on this identifier:

.. sourcecode:: glsl

	vec4 fragmentColour = vec4(1, 1, 1, 1); // Default value 
	if(materialId < 0.5) //Avoid '==' when working with floats.
	{
		fragmentColour = vec4(1, 0, 0, 1) // Draw material 0 as red.
	}
	else if(materialId < 1.5) //Avoid '==' when working with floats.
	{
		fragmentColour = vec4(0, 1, 0, 1) // Draw material 1 as green.
	}
	else if(materialId < 2.5) //Avoid '==' when working with floats.
	{
		fragmentColour = vec4(0, 0, 1, 1) // Draw material 2 as blue.
	}
	.
	.
	.

This is a very simple example, and such use of conditional branching within the shader may not be the best approach as it incurs some performance overhead and becomes unwieldy with a large number of materials. Other approaches include encoding a colour directly into the material identifier, or using the identifier as an index into a texture atlas or array.

Blending between materials
--------------------------
An additional complication when working with smooth voxel terrain is that it is usually desirable to blend smoothly between adjacent voxels with different materials. This situation does not occur with cubic meshes because the texture is considered to be per-face instead of per-vertex, and PolyVox enforces this by ensuring that all the vertices of a given face have the same material.

With a smooth mesh it is possible for each of the three vertices of any given triangle to have different material identifiers. If this is not explicitly handled then the graphics hardware will interpolate these material values across the face of the triangle. Fundamentally, the concept of interpolating between material identifiers does not make sense, because if we have (for example) 1='grass', 2='rock' and 3='sand' then it does not make sense to say rock is the average of grass and sand.

Correctly handling of this is a surprising difficult problem. For now, one approach is described in our article 'Volumetric representation of virtual terrain' which appeared in Game Engine Gems Volume 1 and which is freely available through the Google Books preview here: http://books.google.com/books?id=WNfD2u8nIlIC&lpg=PR1&dq=game%20engine%20gems&pg=PA39#v=onepage&q&f=false

Actual implementation of these material blending approaches is left as an exercise to the reader, though it is possible that in the future we will add some utility functions to PolyVox to assist with tasks such as splitting the mesh or adding the required extra vertex attributes. Our test implementations have performed the mesh processing on the CPU before the mesh is uploaded to the graphics card, but it does seem like there is a lot of potential for implementing these approaches in the geometry shader.

Storage of textures
===================
The other major challenge in texturing voxel based geometry is handling the large number of textures which such environments often require. As an example, a game like Minecraft has hundreds of different material types each with their own texture. The traditional approach to mesh texturing is to bind textures to *texture units* on the GPU before rendering a batch, but even modern GPUs only allow between 16-64 textures to be bound at a time. In this section we discuss various solutions to overcoming this limitation. 

There are various trade offs involved, but if you are targeting hardware with support for *texture arrays* (available from OpenGL 3 and Direct3D 10 onwards) then we can save you some time and tell you that they are almost certainly the best solution. Otherwise you have to understand the various pros and cons of the other approaches described below.

Separate texture units
----------------------
Before we make things unnecessarily complicated, you should consider whether you do actually need the hundreds of textures discussed earlier. If you actually only need a few textures then the simplest solution may indeed be to pass them in via different texture units. You can then select the desired textures using a series of if statements, or a switch statement if the material identifiers are integer values. There is probably some performance overhead here, but you may find it is acceptable for a small number of textures. Keep in mind that you may need to reserve some texture units for additional texture data such as normal maps or shadow maps.

Splitting the mesh
------------------
If your required number of textures do indeed exceed the available number of textures units then one option is to break the mesh down into a number of pieces. Let's say you have a mesh which contains one hundred different materials. As an extreme solution you could break it down into one hundred separate meshes, and for each mesh you could then bind the required single texture before drawing the geometry. Obviously this will dramatically increase the batch count of your scene and so is not recommended.

A more practical approach would be to break the mesh into a smaller number of pieces such that each mesh uses several textures but less than the maximum number of texture units. For example, our mesh with one hundred materials could be split into ten meshes, the first of which contains those triangles using materials 0-9, the seconds contains those triangles using materials 10-19, and so forth. There is a trade off here between the number of batches and the number of textures units used per batch.

Furthermore, you could realise that although your terrain may use hundreds of different textures, any given region is likely to use only a small fraction of those. We have yet to experiment with this, but it seems if you region uses only (for example) materials 12, 47, and 231, then you could conceptually map these materials to the first three textures slots. This means that for each region you draw the mapping between material IDs and texture units would be different. This may require some complex logic in the application but could allow you to do much more with only a few texture units.

Texture atlases
---------------
Probably the most widely used method is to pack a number of textures together into a single large texture, and to our knowledge this is the approach used by Minecraft. For example, if each of your textures are 256x256 texels, and if the maximum texture size supported by your target hardware is 4096x4096 texels, then you can pack 16 x 16 = 256 small textures into the larger one. If this isn't enough (or if your input textures are larger than 256x256) then you can also combine this approach with multiple texture units or with the mesh splitting described previously.

However, there are a number of problems with packing textures like this. Most obviously, it limits the size of your textures as they now have to be significantly smaller then the maximum texture size. Whether this is a problem will really depend on your application.

Next, it means you have to adjust your UV coordinates to correctly address a given texture inside the atlas. UV coordinates for a single texture would normally vary between 0.0 and 1.0 in both dimensions, but when packed into a texture atlas each texture uses only a small part of this range. You will need to apply offsets and scaling factors to your UV coordinates to address your texture correctly.

However, the biggest problem with texture atlases is that they causes problems with texture filtering and with mipmaps. The filtering problem occurs because graphics hardware usually samples the surrounding texels and performs linear interpolation to compute the colour of a given sample point, but when multiple textures are packed together these surrounding texels can actually come from a neighbouring packed texture rather than wrapping round to sample on the other side of the same packed texture. The mipmap problem occurs because for the highest mipmap levels (such as 1x1 or 2x2) multiple textures are being are being averaged together.

It is possible to combat these problems but the solutions are non-trivial. You will want to limit the number of miplevels which you use, and probably provide custom shader code to handle the wrapping of texture coordinates, the sampling of MIP maps, and the calculation of interpolated values. You can also try adding a border around all your packed textures, perhaps by duplicating each texture and offsetting by half its size. Even so, it's not clear to us at this point whether the the various artifacts can be completely removed. Minecraft handles it by completely disabling texture filtering and using the resulting pixelated look as part of its aesthetic.

3D texture slices
-----------------
The idea here is similar to the texture atlas approach, but rather than packing textures side-by-side in an atlas they are instead packed as slices in a 3D texture. We haven't actually tested this but in theory it may have a couple of benefits. Firstly, it simplifies the addressing of the texture as there is no need to offset/scale the UV coordinates, and the W coordinate (the slice index) can be more easily computed from the material identifier. Secondly, a single volume texture will usually be able to hold more texels than a single 2D texture (for example, 512x512x512 is bigger than 4096x4096). Lastly, it should simplify the filtering problem as packed textures are no longer tiled and so should wrap correctly.

However, MIP mapping will probably be more complex than the texture atlas case because even the first MIP level will involve combining adjacent slices. Volume textures are also not so widely supported and may be particularly problematic on mobile hardware.

Texture arrays
--------------
These provide the perfect solution to the problem of handling a large number of textures... at least if they are supported by your hardware. They were introduced with OpenGL 3 and Direct3D 10 but older versions of OpenGL may still be able to access the functionality via extensions. They allow you to bind an array of textures to the shader, and the advantage compared to a texture atlas is that the hardware understands that the textures are separate and so avoids the filtering and mipmapping issues. Beyond the hardware requirements, the only real limitation is that all the textures must be the same size.

Bindless rendering
------------------
We don't have much to say about this option as it needs significant research, but bindless rendering is one of the new OpenGL extensions to come out of Nvidia. The idea is that it removes the abstraction of needing to 'bind' a texture to a particular texture unit, and instead allows more direct access to the texture data on the GPU. This means you can have access to a much larger number of textures from your shader. Sounds useful, but we've yet to investigate it.