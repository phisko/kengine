%module Array
%{
#include "PolyVoxImpl\SubArray.h"
#include "Array.h"
%}

%include "PolyVoxImpl\SubArray.h"
%include "Array.h"

%template(Array3IndexAndMaterial) PolyVox::Array<3, PolyVox::IndexAndMaterial>;
