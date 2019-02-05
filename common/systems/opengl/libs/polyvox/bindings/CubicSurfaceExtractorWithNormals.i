%module CubicSurfaceExtractorWithNormals
%{
#include "CubicSurfaceExtractorWithNormals.h"
%}

%include "CubicSurfaceExtractorWithNormals.h"

%template(CubicSurfaceExtractorWithNormalsSimpleVolumeuint8) PolyVox::CubicSurfaceExtractorWithNormals<PolyVox::SimpleVolume<uint8_t>, PolyVox::DefaultIsQuadNeeded<uint8_t> >;
