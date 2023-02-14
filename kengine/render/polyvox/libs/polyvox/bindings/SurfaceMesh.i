%module SurfaceMesh
%{
#include "Region.h"
#include "Vertex.h"
#include "Mesh.h"
%}

%include "Region.h"
%include "Vertex.h"
%include "Mesh.h"

//%template(VertexTypeVector) std::vector<PolyVox::VertexType>;
//%template(PositionMaterialVector) std::vector<PolyVox::PositionMaterial>;
//%template(PositionMaterialNormalVector) std::vector<PolyVox::PositionMaterialNormal>;
//%template(LodRecordVector) std::vector<PolyVox::LodRecord>;
//%template(uint8Vector) std::vector<uint8_t>;
//%template(uint32Vector) std::vector<uint32_t>;

%template(MeshPositionMaterial) PolyVox::Mesh<PolyVox::CubicVertex<uint8_t>, uint16_t >;
%template(MeshPositionMaterialNormal) PolyVox::Mesh<PolyVox::MarchingCubesVertex<uint8_t>, uint16_t >;