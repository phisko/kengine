%module CubicSurfaceExtractor
%{
#include "CubicSurfaceExtractor.h"
%}

%include "CubicSurfaceExtractor.h"

%template(extractCubicMeshSimpleVolumeuint8) extractCubicMesh<PolyVox::PagedVolume<uint8_t> >;
//EXTRACTORS(CubicSurfaceExtractor)
