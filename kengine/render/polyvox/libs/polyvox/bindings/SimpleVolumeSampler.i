%module SimpleVolumeSampler
%{
#include "SimpleVolume.h"
%}

%include "SimpleVolume.h"

%template(SimpleVolumeSamplerMaterial8) PolyVox::SimpleVolume::Sampler<PolyVox::Material8>;
%template(SimpleVolumeSamplerDensity8) PolyVox::SimpleVolume::Sampler<PolyVox::Density8>;