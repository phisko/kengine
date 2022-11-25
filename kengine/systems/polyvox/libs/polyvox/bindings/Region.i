%module Region
%{
#define SWIG_DAVID
#include "Region.h"
%}

%extend PolyVox::Region {
	STR()
};

%ignore depth;
%ignore height;
%ignore width;
%ignore dimensions;

%include "Region.h"