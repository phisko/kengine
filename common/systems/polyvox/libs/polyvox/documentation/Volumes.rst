*******
Volumes
*******
The volume classes are the heart and soul of Polyvox, providing storage for the huge amounts of data which volumetric environments often require.

Volume concepts
===============

The main volume classes
=======================

Basic access to volume data
===========================
At the simplest level, individual voxels can be read and written by the getVoxel() and setVoxel() member functions which exist for each volume. We will focus on reading voxels first.

Reading voxels
--------------
The easiest way to read a particular voxel is as follows:

.. sourcecode :: c++

 RawVolume<uint8_t>* volume = ... // Get a volume from somewhere.
 uint8_t uVoxelValue = volume->getVoxel(x, y, z); // Read the voxel at the given position.
 
This works great as long as the provided position (x, y, z) does actually represent a valid position inside the volume. If this is not the case then the default behaviour is to throw an exception, so you should be prepared to handle this if you are not certain the position is valid.

Alternatively you can modify the behaviour which occurs when a position is outside the volume by providing a WrapMode as a fourth parameter to getVoxel(). For example:

.. sourcecode :: c++

 // This will return the voxel, or '42' if the position is outside the volume
 uint8_t uVoxelValue = volume->getVoxel(x, y, z, WrapModes::Border, 42);
 
Please see the documentation for WrapMode for more information on the avaiable options here. Note in particular that WrapModes::AssumeValid can be used to skip any bounds checking and so you should use this *if you are certain* that you are accessing a valid position, as it may be noticably faster in some cases.
 
.. sourcecode :: c++

 // This will result in undefined behaviour if the position is outside the volume
 uint8_t uVoxelValue = volume->getVoxel(x, y, z, WrapModes::AssumeValid);
 
Fast access to voxel data is very important in PolyVox, but the above functions have a drawback in that they need to contain logic to evaluate the provided WrapMode and decide how to proceed. This introduces branching into the execution flow and results in larger functions which may prevent inlining. For even more speed you can use version of the above functions which are templatised on the WrapMode rather than taking it as a parameter. This means the condition can be evaluated at compile time rather than run time: For example:
 
.. sourcecode :: c++

 // This will return the voxel, or '42' if the position is outside the volume
 uint8_t uVoxelValue = volume->getVoxel<WrapModes::Border>(x, y, z, 42);
 
Writing voxels
--------------
The setVoxel() function is used for writting to voxels instread of reading from them, and besides this it has two main behavoural differences when compared to getVoxel(). The first difference is that it is not templatised because the speed of setVoxel() is typically less important than getVoxel() (as writing to voxels is much less common than reading from them). However, we could add these temlatised version in the future if they are needed.
 
The second difference is that certain wrap modes such as WrapModes::Border or WrapModes::Clamp do not make much sense when writting to voxel data, and so these are no permitted and will result in an exception being thrown. You should only use WrapModes::Validate (the default) and WrapModes::AssumeValid. For example:
 
.. sourcecode :: c++

 RawVolume<uint8_t>* volume = ... // Get a volume from somewhere.
 volume->setVoxel(x, y, z, 57); // Write the voxel at the given position.
 volume->setVoxel(x, y, z, 57, WrapMopdes::AssumeValid); // No bounds checks
 
Notes on error handling and performance
---------------------------------------
Overall, you should set the wrap mode to WrapModes::AssumeValid for maximum performance (and use templatised versions where available), but note that even this fast version does still contain a POLYVOX_ASSERT() to try and catch mistakes. It appears that this assert prevents inlining (probably due to the logging it performs), but it is anticipated that you will disable such asserts in the final build of your software.