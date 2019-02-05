*********
Threading
*********
Modern computing hardware typically contains a large number of processors, and so users of PolyVox often want to know how they can best make use of these from their applications. 

PolyVox does not make any guarantees about thread-safety, and does not contain any threading primitives to protect access to data structures. You can still make use of PolyVox from multiple threads, but you will have to take responsibility for enforcing thread safety yourself (e.g. by providing thread safe wrappers around the volume classes). If you do want to use PolyVox is a multi-threaded context then this document provides some tips and tricks that you might find useful.

However, be aware that we do not have a lot of expertise in threading, and this is part of the reason why it is not explicitly addressed within PolyVox. If you do have more experience and believe any of this information to be misleading then please do post on the forums to discuss it.

Volumes
=======
Volumes are a core aspect of PolyVox, and so a natural question is whether it is safe to simultaneously access a given volume from multiple threads. The answer to this is actually fairly complex and is also dependent on the type of volume which is being used.

RawVolume
---------
The RawVolume has a very simple internal structure in which the data is stored as a single array which is never moved or resized. Because of this property it is indeed safe to perform multiple simultaneous *reads* of the data from different threads. However, it is generally not safe to perform simultaneous writes from different threads, or even to write from only one thread while other threads are reading.

The reason why simultaneous writes can be problematic should be fairly obvious - if two different threads try to write to the same voxel then the result will depend on which thread writes first. But why can't we write from one thread and read from another one? The problem here is that the the CPU may implement some kind of caching mechanism and/or choose to store data in registers. If a write operation occurs before a read, then the read *may* still obtain the old value because the cache has not been updated yet. There may even be a cache for each thread (particularly if running on multiple processors).

If we assume for a moment that each voxel is a simple integer, then the rules for accessing a single voxel from multiple threads are the same as the rules for accessing integers from multiple threads. There is some useful information about this available on the web, including a discussion on StackOverflow: http://stackoverflow.com/questions/4588915/can-an-integer-be-shared-between-threads-safely

If nothing else, this serves to illustrate that multi-threaded access even to something as simple as an integer can be surprisingly complex and architecture dependant.

However, all this has been in the context of accessing a *single* voxel from multiple threads... what if we carefully design our algorithm such that different threads access different parts of the volume which never overlap? Unfortunately I don't believe this is a solution either. Caching mechanisms seldom operate on individual elements but instead tend to cache larger chunks of data on the grounds that data accesses are usually localised. So it's quite possible that accessing a given voxel will cause another voxel to be cached.

C++ does provide the 'volatile' keyword which can be used to ensure a variable is updated immediately (rather than being cached) but this is still not sufficient for thread safe code. It also has performance implications which we would like to avoid. More information about volatile and multitheaded programming can be found here: http://software.intel.com/en-us/blogs/2007/11/30/volatile-almost-useless-for-multi-threaded-programming/

Lastly, note that PolyVox volumes are templatised which means the voxel type might be something other than a simple int. However we don't think this actually makes a difference given that so few guarantees are made anyway, and it should still be safe to perform multiple concurrent reads for more complex types. 

PagedVolume
-----------
The PagedVolume provides even less thread safety than the RawVolume, in that even concurrent read operations can cause problems. The reason for this is the more complex memory management which is performed behind the scenes, and which allows pieces of volume data to be moved around and deleted. For example, a read of a single voxel may mean that the block of data associated with that voxel has to be paged in to memory, which in turn may mean that another block of data has to be paged out of memory. If second thread was halfway through reading a voxel in this second block of data then a problem will occur.

In the future we may do a more comprehensive analysis of thread safety in the PagedVolume, but for now you should assume that any multithreaded access can cause problems.

Consequences of abuse
---------------------
We have outlined above the rules for multithreaded access of volumes, but what actually happens if you violate these? There's a couple of things to watch out for:

- As mentioned, performing unprotected writes to the volume can cause problems because the data may be copied into the CPU cache and/or registers, and so a subsequent read could retrieve the old value. This is not what you want but probably won't be fatal (i.e. it shouldn't crash). It would basically manifest itself as data corruption.
- If you access the PagedVolume in a multithreaded fashion then you risk trying to access data which has been removed by another thread, and in this case you will get undefined behaviour. This will probably be a crash (out of bounds access) but really anything could happen.

Surface Extraction
==================
Despite the lack of thread safety built in to PolyVox, it is still possible and often desirable to make use of multiple threads for tasks such as surface extraction. Performing surface extraction does not require write access to the data, and we've already established that you can safely perform reads from different threads *provided you are not using the PagedVolume*.

Combining multiple surface extraction threads with the *PagedVolume* is something we will need to experiment with in the future, to determine how it can be improved.

In the future we will expand this section to discuss how to split surface extraction across a number of threads, but for now please see Section 3.4.3 of the book chapter 'Volumetric Representation of Virtual environments', available for free here: http://books.google.nl/books?id=WNfD2u8nIlIC&lpg=PR1&dq=game+engine+gems&pg=PA39&redir_esc=y#v=onepage&q&f=false

GPU thread safety
=================
Be aware that even if you successfully perform surface extraction across multiple threads then you still need to take care when uploading the data to the GPU. For Direct3D 9.0 and OpenGL 2.0 it is only possible to upload data from the main thread (or more accurately the one which owns the rendering context). So after you have performed your multi-threaded surface extraction you need to bring the data back to the main thread for uploading to the GPU.

More recent versions of the Direct3D and OpenGL APIs lift this restriction and provide means of accessing GPU resources from multiple threads. Please consult the documentation for your API for details.

Future work
===========
Threading support is not a high priority for PolyVox because it can be implemented by the user at a higher level. However, there are a couple of areas we may investigate in the future.

Thread safe volume wrapper
--------------------------
It might be useful to provide a thread safe wrapper around the volume classes, and this could possibly be included in the PolyVox utilities or as a extra library. This thread safe wrapper could be templatised to work with any internal volume type, and could itself be a volume so that it can be used directly with the existing algorithms.

OpenMP
------
This is a standard for extending C++ with compiler directives which allow the compiler to automatically parallelise sections of code. Most likely this could be used to parallelise some of the loops which occur in image processing tasks.