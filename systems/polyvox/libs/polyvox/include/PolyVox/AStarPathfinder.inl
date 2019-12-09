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

#include "Impl/ErrorHandling.h"

namespace PolyVox
{
	////////////////////////////////////////////////////////////////////////////////
	// Useful constants
	////////////////////////////////////////////////////////////////////////////////
	const float sqrt_1 = 1.0f;
	const float sqrt_2 = 1.4143f;
	const float sqrt_3 = 1.7321f;

	const Vector3DInt32 arrayPathfinderFaces[6] =
	{
		Vector3DInt32(0, 0, -1),
		Vector3DInt32(0, 0, +1),
		Vector3DInt32(0, -1, 0),
		Vector3DInt32(0, +1, 0),
		Vector3DInt32(-1, 0, 0),
		Vector3DInt32(+1, 0, 0)
	};

	const Vector3DInt32 arrayPathfinderEdges[12] =
	{
		Vector3DInt32(0, -1, -1),
		Vector3DInt32(0, -1, +1),
		Vector3DInt32(0, +1, -1),
		Vector3DInt32(0, +1, +1),
		Vector3DInt32(-1, 0, -1),
		Vector3DInt32(-1, 0, +1),
		Vector3DInt32(+1, 0, -1),
		Vector3DInt32(+1, 0, +1),
		Vector3DInt32(-1, -1, 0),
		Vector3DInt32(-1, +1, 0),
		Vector3DInt32(+1, -1, 0),
		Vector3DInt32(+1, +1, 0)
	};

	const Vector3DInt32 arrayPathfinderCorners[8] =
	{
		Vector3DInt32(-1, -1, -1),
		Vector3DInt32(-1, -1, +1),
		Vector3DInt32(-1, +1, -1),
		Vector3DInt32(-1, +1, +1),
		Vector3DInt32(+1, -1, -1),
		Vector3DInt32(+1, -1, +1),
		Vector3DInt32(+1, +1, -1),
		Vector3DInt32(+1, +1, +1)
	};

	////////////////////////////////////////////////////////////////////////////////
	/// Using this function, a voxel is considered valid for the path if it is inside the
	/// volume and if its density is below that returned by the voxel's getDensity() function.
	/// \return true is the voxel is valid for the path
	////////////////////////////////////////////////////////////////////////////////
	template<typename VolumeType>
	bool aStarDefaultVoxelValidator(const VolumeType* volData, const Vector3DInt32& v3dPos)
	{
		//Voxels are considered valid candidates for the path if they are inside the volume...
		if (volData->getEnclosingRegion().containsPoint(v3dPos) == false)
		{
			return false;
		}

		return true;
	}

	////////////////////////////////////////////////////////////////////////////////
	// AStarPathfinder Class
	////////////////////////////////////////////////////////////////////////////////
	template<typename VolumeType>
	AStarPathfinder<VolumeType>::AStarPathfinder(const AStarPathfinderParams<VolumeType>& params)
		:m_params(params)
	{
	}

	template<typename VolumeType>
	void AStarPathfinder<VolumeType>::execute()
	{
		//Clear any existing nodes
		allNodes.clear();
		openNodes.clear();
		closedNodes.clear();

		//Clear the result
		m_params.result->clear();

		//Iterators to start and end node.
		AllNodesContainer::iterator startNode = allNodes.insert(Node(m_params.start.getX(), m_params.start.getY(), m_params.start.getZ())).first;
		AllNodesContainer::iterator endNode = allNodes.insert(Node(m_params.end.getX(), m_params.end.getY(), m_params.end.getZ())).first;

		//Regarding the const_cast - normally you should not modify an object which is in an sdt::set.
		//The reason is that objects in a set are stored sorted in a tree so they can be accessed quickly,
		//and changing the object directly can break the sorting. However, in our case we have provided a
		//custom sort operator for the set which we know only uses the position to sort. Hence we can safely
		//modify other properties of the object while it is in the set.
		Node* tempStart = const_cast<Node*>(&(*startNode));
		tempStart->gVal = 0;
		tempStart->hVal = computeH(startNode->position, endNode->position);

		Node* tempEnd = const_cast<Node*>(&(*endNode));
		tempEnd->hVal = 0.0f;

		openNodes.insert(startNode);

		float fDistStartToEnd = (endNode->position - startNode->position).length();
		m_fProgress = 0.0f;
		if (m_params.progressCallback)
		{
			m_params.progressCallback(m_fProgress);
		}

		while ((openNodes.empty() == false) && (openNodes.getFirst() != endNode))
		{
			//Move the first node from open to closed.
			current = openNodes.getFirst();
			openNodes.removeFirst();
			closedNodes.insert(current);

			//Update the user on our progress
			if (m_params.progressCallback)
			{
				const float fMinProgresIncreament = 0.001f;
				float fDistCurrentToEnd = (endNode->position - current->position).length();
				float fDistNormalised = fDistCurrentToEnd / fDistStartToEnd;
				float fProgress = 1.0f - fDistNormalised;
				if (fProgress >= m_fProgress + fMinProgresIncreament)
				{
					m_fProgress = fProgress;
					m_params.progressCallback(m_fProgress);
				}
			}

			//The distance from one cell to another connected by face, edge, or corner.
			const float fFaceCost = sqrt_1;
			const float fEdgeCost = sqrt_2;
			const float fCornerCost = sqrt_3;

			//Process the neighbours. Note the deliberate lack of 'break' 
			//statements, larger connectivities include smaller ones.
			switch (m_params.connectivity)
			{
			case TwentySixConnected:
				processNeighbour(current->position + arrayPathfinderCorners[0], current->gVal + fCornerCost);
				processNeighbour(current->position + arrayPathfinderCorners[1], current->gVal + fCornerCost);
				processNeighbour(current->position + arrayPathfinderCorners[2], current->gVal + fCornerCost);
				processNeighbour(current->position + arrayPathfinderCorners[3], current->gVal + fCornerCost);
				processNeighbour(current->position + arrayPathfinderCorners[4], current->gVal + fCornerCost);
				processNeighbour(current->position + arrayPathfinderCorners[5], current->gVal + fCornerCost);
				processNeighbour(current->position + arrayPathfinderCorners[6], current->gVal + fCornerCost);
				processNeighbour(current->position + arrayPathfinderCorners[7], current->gVal + fCornerCost);

			case EighteenConnected:
				processNeighbour(current->position + arrayPathfinderEdges[0], current->gVal + fEdgeCost);
				processNeighbour(current->position + arrayPathfinderEdges[1], current->gVal + fEdgeCost);
				processNeighbour(current->position + arrayPathfinderEdges[2], current->gVal + fEdgeCost);
				processNeighbour(current->position + arrayPathfinderEdges[3], current->gVal + fEdgeCost);
				processNeighbour(current->position + arrayPathfinderEdges[4], current->gVal + fEdgeCost);
				processNeighbour(current->position + arrayPathfinderEdges[5], current->gVal + fEdgeCost);
				processNeighbour(current->position + arrayPathfinderEdges[6], current->gVal + fEdgeCost);
				processNeighbour(current->position + arrayPathfinderEdges[7], current->gVal + fEdgeCost);
				processNeighbour(current->position + arrayPathfinderEdges[8], current->gVal + fEdgeCost);
				processNeighbour(current->position + arrayPathfinderEdges[9], current->gVal + fEdgeCost);
				processNeighbour(current->position + arrayPathfinderEdges[10], current->gVal + fEdgeCost);
				processNeighbour(current->position + arrayPathfinderEdges[11], current->gVal + fEdgeCost);

			case SixConnected:
				processNeighbour(current->position + arrayPathfinderFaces[0], current->gVal + fFaceCost);
				processNeighbour(current->position + arrayPathfinderFaces[1], current->gVal + fFaceCost);
				processNeighbour(current->position + arrayPathfinderFaces[2], current->gVal + fFaceCost);
				processNeighbour(current->position + arrayPathfinderFaces[3], current->gVal + fFaceCost);
				processNeighbour(current->position + arrayPathfinderFaces[4], current->gVal + fFaceCost);
				processNeighbour(current->position + arrayPathfinderFaces[5], current->gVal + fFaceCost);
			}

			if (allNodes.size() > m_params.maxNumberOfNodes)
			{
				//We've reached the specified maximum number
				//of nodes. Just give up on the search.
				break;
			}
		}

		if ((openNodes.empty()) || (openNodes.getFirst() != endNode))
		{
			//In this case we failed to find a valid path.
			POLYVOX_THROW(std::runtime_error, "No path found");
		}
		else
		{
			//Regarding the const_cast - normally you should not modify an object which is in an sdt::set.
			//The reason is that objects in a set are stored sorted in a tree so they can be accessed quickly,
			//and changing the object directly can break the sorting. However, in our case we have provided a
			//custom sort operator for the set which we know only uses the position to sort. Hence we can safely
			//modify other properties of the object while it is in the set.
			Node* n = const_cast<Node*>(&(*endNode));
			while (n != 0)
			{
				m_params.result->push_front(n->position);
				n = n->parent;
			}
		}

		if (m_params.progressCallback)
		{
			m_params.progressCallback(1.0f);
		}
	}

	template<typename VolumeType>
	void AStarPathfinder<VolumeType>::processNeighbour(const Vector3DInt32& neighbourPos, float neighbourGVal)
	{
		bool bIsVoxelValidForPath = m_params.isVoxelValidForPath(m_params.volume, neighbourPos);
		if (!bIsVoxelValidForPath)
		{
			return;
		}

		float cost = neighbourGVal;

		std::pair<AllNodesContainer::iterator, bool> insertResult = allNodes.insert(Node(neighbourPos.getX(), neighbourPos.getY(), neighbourPos.getZ()));
		AllNodesContainer::iterator neighbour = insertResult.first;

		if (insertResult.second == true) //New node, compute h.
		{
			Node* tempNeighbour = const_cast<Node*>(&(*neighbour));
			tempNeighbour->hVal = computeH(neighbour->position, m_params.end);
		}

		OpenNodesContainer::iterator openIter = openNodes.find(neighbour);
		if (openIter != openNodes.end())
		{
			if (cost < neighbour->gVal)
			{
				openNodes.remove(openIter);
				openIter = openNodes.end();
			}
		}

		//TODO - Nodes could keep track of if they are in open or closed? And a pointer to where they are?
		ClosedNodesContainer::iterator closedIter = closedNodes.find(neighbour);
		if (closedIter != closedNodes.end())
		{
			if (cost < neighbour->gVal)
			{
				//Probably shouldn't happen?
				closedNodes.remove(closedIter);
				closedIter = closedNodes.end();
			}
		}

		if ((openIter == openNodes.end()) && (closedIter == closedNodes.end()))
		{
			//Regarding the const_cast - normally you should not modify an object which is in an sdt::set.
			//The reason is that objects in a set are stored sorted in a tree so they can be accessed quickly,
			//and changing the object directly can break the sorting. However, in our case we have provided a
			//custom sort operator for the set which we know only uses the position to sort. Hence we can safely
			//modify other properties of the object while it is in the set.
			Node* temp = const_cast<Node*>(&(*neighbour));
			temp->gVal = cost;
			openNodes.insert(neighbour);
			temp->parent = const_cast<Node*>(&(*current));
		}
	}

	template<typename VolumeType>
	float AStarPathfinder<VolumeType>::SixConnectedCost(const Vector3DInt32& a, const Vector3DInt32& b)
	{
		//This is the only heuristic I'm sure of - just use the manhatten distance for the 6-connected case.
		uint32_t faceSteps = std::abs(a.getX() - b.getX()) + std::abs(a.getY() - b.getY()) + std::abs(a.getZ() - b.getZ());

		return faceSteps * 1.0f;
	}

	template<typename VolumeType>
	float AStarPathfinder<VolumeType>::EighteenConnectedCost(const Vector3DInt32& a, const Vector3DInt32& b)
	{
		//I'm not sure of the correct heuristic for the 18-connected case, so I'm just letting it fall through to the 
		//6-connected case. This means 'h' will be bigger than it should be, resulting in a faster path which may not 
		//actually be the shortest one. If you have a correct heuristic for the 18-connected case then please let me know.

		return SixConnectedCost(a, b);
	}

	template<typename VolumeType>
	float AStarPathfinder<VolumeType>::TwentySixConnectedCost(const Vector3DInt32& a, const Vector3DInt32& b)
	{
		//Can't say I'm certain about this heuristic - if anyone has
		//a better idea of what it should be then please let me know.
		uint32_t array[3];
		array[0] = std::abs(a.getX() - b.getX());
		array[1] = std::abs(a.getY() - b.getY());
		array[2] = std::abs(a.getZ() - b.getZ());

		//Maybe this is better implemented directly
		//using three compares and two swaps... but not
		//until the profiler says so.
		std::sort(&array[0], &array[3]);

		uint32_t cornerSteps = array[0];
		uint32_t edgeSteps = array[1] - array[0];
		uint32_t faceSteps = array[2] - array[1];

		return cornerSteps * sqrt_3 + edgeSteps * sqrt_2 + faceSteps * sqrt_1;
	}

	template<typename VolumeType>
	float AStarPathfinder<VolumeType>::computeH(const Vector3DInt32& a, const Vector3DInt32& b)
	{
		float hVal;

		switch (m_params.connectivity)
		{
		case TwentySixConnected:
			hVal = TwentySixConnectedCost(a, b);
			break;
		case EighteenConnected:
			hVal = EighteenConnectedCost(a, b);
			break;
		case SixConnected:
			hVal = SixConnectedCost(a, b);
			break;
		default:
			POLYVOX_THROW(std::invalid_argument, "Connectivity parameter has an unrecognised value.");
		}

		//Sanity checks in debug mode. These can come out eventually, but I
		//want to make sure that the heuristics I've come up with make sense.
		POLYVOX_ASSERT((a - b).length() <= TwentySixConnectedCost(a, b), "A* heuristic error.");
		POLYVOX_ASSERT(TwentySixConnectedCost(a, b) <= EighteenConnectedCost(a, b), "A* heuristic error.");
		POLYVOX_ASSERT(EighteenConnectedCost(a, b) <= SixConnectedCost(a, b), "A* heuristic error.");

		//Apply the bias to the computed h value;
		hVal *= m_params.hBias;

		//Having computed hVal, we now apply some random bias to break ties.
		//This needs to be deterministic on the input position. This random
		//bias means it is much les likely that two paths are exactly the same
		//length, and so far fewer nodes must be expanded to find the shortest path.
		//See http://theory.stanford.edu/~amitp/GameProgramming/Heuristics.html#S12

		//Note that if the hash is zero we can have differences between the Linux vs. Windows
		//(or perhaps GCC vs. VS) versions of the code. This is probably because of the way
		//sorting inside the std::set works (i.e. one system swaps values which are identical
		//while the other one doesn't - both approaches are valid). For the same reason we want
		//to make sure that position (x,y,z) has a differnt hash from e.g. position (x,z,y).
		uint32_t aX = (a.getX() << 16) & 0x00FF0000;
		uint32_t aY = (a.getY() << 8) & 0x0000FF00;
		uint32_t aZ = (a.getZ()) & 0x000000FF;
		uint32_t hashVal = hash(aX | aY | aZ);

		//Stop hashVal going over 65535, and divide by 1000000 to make sure it is small.
		hashVal &= 0x0000FFFF;
		float fHash = hashVal / 1000000.0f;

		//Apply the hash and return
		hVal += fHash;
		return hVal;
	}

	// Robert Jenkins' 32 bit integer hash function
	// http://www.burtleburtle.net/bob/hash/integer.html
	template<typename VolumeType>
	uint32_t AStarPathfinder<VolumeType>::hash(uint32_t a)
	{
		a = (a + 0x7ed55d16) + (a << 12);
		a = (a ^ 0xc761c23c) ^ (a >> 19);
		a = (a + 0x165667b1) + (a << 5);
		a = (a + 0xd3a2646c) ^ (a << 9);
		a = (a + 0xfd7046c5) + (a << 3);
		a = (a ^ 0xb55a4f09) ^ (a >> 16);
		return a;
	}
}
