%module PagedVolume

#pragma SWIG nowarn=SWIGWARN_PARSE_NESTED_CLASS

namespace PolyVox
{
	class PolyVox::PagedVolume_Chunk {
	public:
		PagedVolume_Chunk(Vector3DInt32 v3dPosition, uint16_t uSideLength, PolyVox::PagedVolume_Pager* pPager = nullptr);
		~PagedVolume_Chunk();

		VoxelType* getData(void) const;
		uint32_t getDataSizeInBytes(void) const;

		VoxelType getVoxel(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos) const;
		VoxelType getVoxel(const Vector3DUint16& v3dPos) const;

		void setVoxelAt(uint16_t uXPos, uint16_t uYPos, uint16_t uZPos, VoxelType tValue);
		void setVoxelAt(const Vector3DUint16& v3dPos, VoxelType tValue);
	};
	
	class PolyVox::PagedVolume_Pager {
	public:
		/// Constructor
		PagedVolume_Pager() {};
		/// Destructor
		virtual ~PagedVolume_Pager() {};

		virtual void pageIn(const Region& region, PagedVolume_Chunk* pChunk) = 0;
		virtual void pageOut(const Region& region, PagedVolume_Chunk* pChunk) = 0;
	};
}

%{
#include "PagedVolume.h"
%}

%include "PagedVolume.h"

%{
namespace PolyVox
{
	// SWIG thinks that Inner is a global class, so we need to trick the C++
	// compiler into understanding this so called global type.
	typedef PagedVolume<int8_t>::Pager PagedVolume_Pager;
	typedef PagedVolume<int8_t>::Chunk PagedVolume_Chunk;
}
%}

VOLUMETYPES(PagedVolume)
