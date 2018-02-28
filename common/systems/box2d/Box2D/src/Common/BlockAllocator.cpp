/*
* Copyright (c) 2006-2009 Erin Catto http://www.box2d.org
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#include <Box2D/Common/BlockAllocator.hpp>
#include <climits>
#include <cstring>
#include <cstddef>

namespace b2
{

int32 BlockAllocator::s_blockSizes[blockSizes] = 
{
	16,		// 0
	32,		// 1
	64,		// 2
	96,		// 3
	128,	// 4
	160,	// 5
	192,	// 6
	224,	// 7
	256,	// 8
	320,	// 9
	384,	// 10
	448,	// 11
	512,	// 12
	640,	// 13
};
uint8 BlockAllocator::s_blockSizeLookup[maxBlockSize + 1];
bool BlockAllocator::s_blockSizeLookupInitialized;

struct Chunk
{
	int32 blockSize;
	Block* blocks;
};

struct Block
{
	Block* next;
};

BlockAllocator::BlockAllocator()
{
	assert(blockSizes < UCHAR_MAX);

	m_chunkSpace = chunkArrayIncrement;
	m_chunkCount = 0;
	m_chunks = (Chunk*)Alloc(m_chunkSpace * sizeof(Chunk));
	
	std::memset(m_chunks, 0, m_chunkSpace * sizeof(Chunk));
	std::memset(m_freeLists, 0, sizeof(m_freeLists));

	if (s_blockSizeLookupInitialized == false)
	{
		int32 j = 0;
		for (int32 i = 1; i <= maxBlockSize; ++i)
		{
			assert(j < blockSizes);
			if (i <= s_blockSizes[j])
			{
				s_blockSizeLookup[i] = (uint8)j;
			}
			else
			{
				++j;
				s_blockSizeLookup[i] = (uint8)j;
			}
		}

		s_blockSizeLookupInitialized = true;
	}
}

BlockAllocator::~BlockAllocator()
{
	for (int32 i = 0; i < m_chunkCount; ++i)
	{
		b2::Free(m_chunks[i].blocks);
	}

	b2::Free(m_chunks);
}

void* BlockAllocator::Allocate(int32 size)
{
	if (size == 0)
		return NULL;

	assert(0 < size);

	if (size > maxBlockSize)
	{
		return Alloc(size);
	}

	int32 index = s_blockSizeLookup[size];
	assert(0 <= index && index < blockSizes);

	if (m_freeLists[index])
	{
		Block* block = m_freeLists[index];
		m_freeLists[index] = block->next;
		return block;
	}
	else
	{
		if (m_chunkCount == m_chunkSpace)
		{
			Chunk* oldChunks = m_chunks;
			m_chunkSpace += chunkArrayIncrement;
			m_chunks = (Chunk*)Alloc(m_chunkSpace * sizeof(Chunk));
			std::memcpy(m_chunks, oldChunks, m_chunkCount * sizeof(Chunk));
			std::memset(m_chunks + m_chunkCount, 0, chunkArrayIncrement * sizeof(Chunk));
			b2::Free(oldChunks);
		}

		Chunk* chunk = m_chunks + m_chunkCount;
		chunk->blocks = (Block*)Alloc(chunkSize);
#if defined(_DEBUG)
		std::memset(chunk->blocks, 0xcd, chunkSize);
#endif
		int32 blockSize = s_blockSizes[index];
		chunk->blockSize = blockSize;
		int32 blockCount = chunkSize / blockSize;
		assert(blockCount * blockSize <= chunkSize);
		for (int32 i = 0; i < blockCount - 1; ++i)
		{
			Block* block = (Block*)((int8*)chunk->blocks + blockSize * i);
			Block* next = (Block*)((int8*)chunk->blocks + blockSize * (i + 1));
			block->next = next;
		}
		Block* last = (Block*)((int8*)chunk->blocks + blockSize * (blockCount - 1));
		last->next = NULL;

		m_freeLists[index] = chunk->blocks->next;
		++m_chunkCount;

		return chunk->blocks;
	}
}

void BlockAllocator::Free(void* p, int32 size)
{
	if (size == 0)
	{
		return;
	}

	assert(0 < size);

	if (size > maxBlockSize)
	{
		b2::Free(p);
		return;
	}

	int32 index = s_blockSizeLookup[size];
	assert(0 <= index && index < blockSizes);

#ifdef _DEBUG
	// Verify the memory address and size is valid.
	int32 blockSize = s_blockSizes[index];
	bool found = false;
	for (int32 i = 0; i < m_chunkCount; ++i)
	{
		Chunk* chunk = m_chunks + i;
		if (chunk->blockSize != blockSize)
		{
			assert(	(int8*)p + blockSize <= (int8*)chunk->blocks ||
					(int8*)chunk->blocks + chunkSize <= (int8*)p);
		}
		else
		{
			if ((int8*)chunk->blocks <= (int8*)p && (int8*)p + blockSize <= (int8*)chunk->blocks + chunkSize)
			{
				found = true;
			}
		}
	}

	assert(found);

	std::memset(p, 0xfd, blockSize);
#endif

	Block* block = (Block*)p;
	block->next = m_freeLists[index];
	m_freeLists[index] = block;
}

void BlockAllocator::Clear()
{
	for (int32 i = 0; i < m_chunkCount; ++i)
	{
		b2::Free(m_chunks[i].blocks);
	}

	m_chunkCount = 0;
	std::memset(m_chunks, 0, m_chunkSpace * sizeof(Chunk));

	std::memset(m_freeLists, 0, sizeof(m_freeLists));
}

} // namespace b2
