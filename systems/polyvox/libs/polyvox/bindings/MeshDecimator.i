%module MeshDecimator
%{
#include "MeshDecimator.h"
%}

%include "MeshDecimator.h"

%template(MeshDecimatorMaterial8) PolyVox::MeshDecimator<PolyVox::Material8>;
%template(MeshDecimatorDensity8) PolyVox::MeshDecimator<PolyVox::Density8>;