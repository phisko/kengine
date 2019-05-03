%module PolyVoxCore

#define POLYVOX_API
%include "Impl/PlatformDefinitions.h"
#define __attribute__(x) //Silence DEPRECATED errors

//This macro allows us to use Python properties on our classes
%define PROPERTY(type,name,getter,setter)
%extend type {
	%pythoncode %{
		__swig_getmethods__["name"] = getter
		__swig_setmethods__["name"] = setter
		if _newclass: name = property(getter, setter)
	%}
};
%enddef

//Put this in an %extend section to wrap operator<< as __str__
%define STR()
const char* __str__() {
	std::ostringstream out;
	out << *$self;
	return out.str().c_str();
}
%enddef

//Centralise this to avoid repeating ourselves
//This macro will be called in the volume interface files to define the various volume types.
%define VOLUMETYPES(class)
%template(class ## int8) PolyVox::class<int8_t>;
//%template(class ## int16) PolyVox::class<int16_t>;
//%template(class ## int32) PolyVox::class<int32_t>;
//%template(class ## uint8) PolyVox::class<uint8_t>;
//%template(class ## uint16) PolyVox::class<uint16_t>;
//%template(class ## uint32) PolyVox::class<uint32_t>;
//%template(class ## float) PolyVox::class<float>;
%enddef

//Template based on voxel type
%define EXTRACTOR(class, volumetype)
%template(class ## volumetype ## int8) PolyVox::class<PolyVox::volumetype<int8_t> >;
//%template(class ## volumetype ## int16) PolyVox::class<PolyVox::volumetype<int16_t> >;
//%template(class ## volumetype ## int32) PolyVox::class<PolyVox::volumetype<int32_t> >;
//%template(class ## volumetype ## uint8) PolyVox::class<PolyVox::volumetype<uint8_t> >;
//%template(class ## volumetype ## uint16) PolyVox::class<PolyVox::volumetype<uint16_t> >;
//%template(class ## volumetype ## uint32) PolyVox::class<PolyVox::volumetype<uint32_t> >;
//%template(class ## volumetype ## float) PolyVox::class<PolyVox::volumetype<float> >;
%enddef

//Template based on volume type
%define EXTRACTORS(shortname)
EXTRACTOR(shortname, PagedVolume)
EXTRACTOR(shortname, RawVolume)
%enddef

%feature("autodoc", "1");

#ifdef SWIGPYTHON
//This will rename "operator=" to "assign" since Python doesn't have assignment
%rename(assign) *::operator=;
#endif
#ifdef SWIGCSHARP
//These operators are not wrappable in C# and their function is provided by other means
%ignore *::operator=;
%ignore *::operator+=;
%ignore *::operator-=;
%ignore *::operator*=;
%ignore *::operator/=;
%ignore *::operator<<; //This is covered by STR()
#endif

%include "stdint.i"
%include "std_vector.i"
%include "Vector.i"
%include "DefaultMarchingCubesController.i"
%include "Region.i"
//%include "Chunk.i"
//%include "CompressedBlock.i"
//%include "UncompressedBlock.i"
//%include "BlockCompressor.i"
//%include "Pager.i"
//%include "FilePager.i"
//%include "MinizBlockCompressor.i"
//%include "RLEBlockCompressor.i"
%include "BaseVolume.i"
//%include "RawVolume.i"
//%include "PagedVolume.i"
//%include "VertexTypes.i"
//%include "SurfaceMesh.i"
////%include "MarchingCubesSurfaceExtractor.i"
////%include "CubicSurfaceExtractor.i"
//%include "Raycast.i"
//%include "Picking.i"
