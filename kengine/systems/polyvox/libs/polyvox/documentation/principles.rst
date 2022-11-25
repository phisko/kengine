**********************
Principles of PolyVox
**********************

.. warning ::
	This section is yet to be fully written and is just a skeleton for now.

PolyVox provides a library for managing 3D arrays (volumes) of data (voxels). It gives you the tools to iterate through, randomly access, read and write volumes. The volumes are templatized on voxel type, so that each voxel can be as simple as just an number or as complex as a class with a range of properties.

Once you have created a volume, PolyVox provides a number of tools for turning it into a mesh that you can pass to your rendering engine. These are called `surface extractors`. A 'Marching Cubes' surface extractor and a 'cubic' (Minecraft-style) surface extractor are included, and it is possible to write your own.

PolyVox primarily handles this task of storing volume data and extracting surfaces from it. Most other aspects of you voxel application or game will need to be implemented by you (rendering, logic, physics, AI, networking, etc) though PolyVox does have a few utility classes for things like the A* algorithm on a 3D grid.