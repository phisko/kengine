/*******************************************************************************
* The MIT License (MIT)
*
* Copyright (c) 2015 David Williams and Matthew Williams
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*******************************************************************************/

#ifndef __PolyVox_AStarPathfinderImpl_H__
#define __PolyVox_AStarPathfinderImpl_H__

#include "../Vector.h"

#include <algorithm>
#include <limits> //For numeric_limits
#include <set>
#include <vector>

namespace PolyVox
{
	class OpenNodesContainer;
	class ClosedNodesContainer;

	/// The Connectivity of a voxel determines how many neighbours it has.
	enum Connectivity
	{
		/// Each voxel has six neighbours, which are those sharing a face.
		SixConnected,
		/// Each voxel has 18 neighbours, which are those sharing a face or an edge.
		EighteenConnected,
		/// Each voxel has 26 neighbours, which are those sharing a face, edge, or corner.
		TwentySixConnected
	};

	struct Node
	{
		Node(int x, int y, int z)
			:gVal(std::numeric_limits<float>::quiet_NaN()) //Initilise with NaNs so that we will
			, hVal(std::numeric_limits<float>::quiet_NaN()) //know if we forget to set these properly.
			, parent(0)
		{
			position.setX(x);
			position.setY(y);
			position.setZ(z);
		}

		bool operator==(const Node& rhs) const
		{
			return position == rhs.position;
		}

		bool operator<(const Node& rhs) const
		{
			if (position.getX() < rhs.position.getX())
				return true;
			if (rhs.position.getX() < position.getX())
				return false;

			if (position.getY() < rhs.position.getY())
				return true;
			if (rhs.position.getY() < position.getY())
				return false;

			if (position.getZ() < rhs.position.getZ())
				return true;
			if (rhs.position.getZ() < position.getZ())
				return false;

			return false;
		}

		PolyVox::Vector3DInt32 position;
		float gVal;
		float hVal;
		Node* parent;

		float f(void) const
		{
			return gVal + hVal;
		}
	};

	typedef std::set<Node> AllNodesContainer;

	class AllNodesContainerIteratorComparator
	{
	public:
		bool operator() (const AllNodesContainer::iterator& lhs, const  AllNodesContainer::iterator& rhs) const
		{
			return (&(*lhs)) < (&(*rhs));
		}
	};

	class NodeSort
	{
	public:
		bool operator() (const AllNodesContainer::iterator& lhs, const AllNodesContainer::iterator& rhs) const
		{
			return lhs->f() > rhs->f();
		}
	};

	class OpenNodesContainer
	{
	public:
		typedef std::vector<AllNodesContainer::iterator>::iterator iterator;

	public:
		void clear(void)
		{
			open.clear();
		}

		bool empty(void) const
		{
			return open.empty();
		}

		void insert(AllNodesContainer::iterator node)
		{
			open.push_back(node);
			push_heap(open.begin(), open.end(), NodeSort());
		}

		AllNodesContainer::iterator getFirst(void)
		{
			return open[0];
		}

		void removeFirst(void)
		{
			pop_heap(open.begin(), open.end(), NodeSort());
			open.pop_back();
		}

		void remove(iterator iterToRemove)
		{
			open.erase(iterToRemove);
			make_heap(open.begin(), open.end(), NodeSort());
		}

		iterator begin(void)
		{
			return open.begin();
		}

		iterator end(void)
		{
			return open.end();
		}

		iterator find(AllNodesContainer::iterator node)
		{
			std::vector<AllNodesContainer::iterator>::iterator openIter = std::find(open.begin(), open.end(), node);
			return openIter;
		}

	private:
		std::vector<AllNodesContainer::iterator> open;
	};

	class ClosedNodesContainer
	{
	public:
		typedef std::set<AllNodesContainer::iterator, AllNodesContainerIteratorComparator>::iterator iterator;

	public:
		void clear(void)
		{
			closed.clear();
		}

		void insert(AllNodesContainer::iterator node)
		{
			closed.insert(node);
		}

		void remove(iterator iterToRemove)
		{
			closed.erase(iterToRemove);
		}

		iterator begin(void)
		{
			return closed.begin();
		}

		iterator end(void)
		{
			return closed.end();
		}

		iterator find(AllNodesContainer::iterator node)
		{
			iterator iter = std::find(closed.begin(), closed.end(), node);
			return iter;
		}

	private:
		std::set<AllNodesContainer::iterator, AllNodesContainerIteratorComparator> closed;
	};


	//bool operator<(const AllNodesContainer::iterator& lhs, const  AllNodesContainer::iterator& rhs);
}

#endif //__PolyVox_AStarPathfinderImpl_H__
