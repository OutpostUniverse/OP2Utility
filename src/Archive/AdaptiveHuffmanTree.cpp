#include "AdaptiveHuffmanTree.h"
#include <utility>
#include <string>
#include <stdexcept>

namespace Archive
{
	// Creates an (adaptive) Huffman tree with terminalNodeCount at the bottom.
	AdaptiveHuffmanTree::AdaptiveHuffmanTree(NodeType terminalNodeCount) :
		// Initialize tree properties
		terminalNodeCount(terminalNodeCount),
		nodeCount(terminalNodeCount * 2 - 1),
		rootNodeIndex(nodeCount - 1),
		// Allocate space for tree
		linkOrData(nodeCount),
		subtreeCount(nodeCount),
		parentIndex(nodeCount + terminalNodeCount)
	{
		// Initialize the tree
		// Initialize terminal nodes
		for (NodeIndex i = 0; i < terminalNodeCount; ++i)
		{
			linkOrData[i] = i + nodeCount;						// Initilize data values
			subtreeCount[i] = 1;
			parentIndex[i] = (i >> 1) + terminalNodeCount;
			parentIndex[i + nodeCount] = i;						// "Parent of code" (node index)
		}
		// Initialize non terminal nodes
		NodeIndex left = 0;
		for (NodeIndex i = terminalNodeCount; i < nodeCount; ++i)
		{
			linkOrData[i] = left;								// Initialize link values
			subtreeCount[i] = subtreeCount[left] + subtreeCount[left + 1];	// Count is sum of two subtrees
			parentIndex[i] = (i >> 1) + terminalNodeCount;
			left += 2;										// Calc index of left child (next loop)
		}
	}



	AdaptiveHuffmanTree::NodeType AdaptiveHuffmanTree::TerminalNodeCount()
	{
		return terminalNodeCount;
	}

	// Returns the index of the root node.
	// All tree searches start at the root node.
	AdaptiveHuffmanTree::NodeIndex AdaptiveHuffmanTree::GetRootNodeIndex()
	{
		return rootNodeIndex;
	}

	// Return a child node of the given node.
	// This is used to traverse the tree to a terminal node.
	AdaptiveHuffmanTree::NodeIndex AdaptiveHuffmanTree::GetChildNode(NodeIndex nodeIndex, bool bRight)
	{
		VerifyNodeIndexInBounds(nodeIndex);

		// Return the child node index
		return linkOrData[nodeIndex] + bRight;
	}

	// Returns true if the node is a terminal node.
	// This is used to know when a tree search has completed.
	bool AdaptiveHuffmanTree::IsLeaf(NodeIndex nodeIndex)
	{
		VerifyNodeIndexInBounds(nodeIndex);

		// Return whether or not this is a terminal node
		return linkOrData[nodeIndex] >= nodeCount;
	}

	// Returns the data stored in a terminal node
	AdaptiveHuffmanTree::NodeData AdaptiveHuffmanTree::GetNodeData(NodeIndex nodeIndex)
	{
		VerifyNodeIndexInBounds(nodeIndex);

		// Return data stored in node translated back to normal form
		// Note: This assumes the node is a terminal node
		return linkOrData[nodeIndex] - nodeCount;
	}




	// This updates the count for the given code and restructures the tree if needed.
	// This is used after a tree search to update the tree (gives more frequently used
	// codes a shorter bit encoding).
	void AdaptiveHuffmanTree::UpdateCodeCount(NodeData code)
	{
		VerifyNodeDataInBounds(code);

		// Get the index of the node containing this code
		NodeIndex curNodeIndex = parentIndex[code + nodeCount];
		subtreeCount[curNodeIndex]++; // Update the node count

		// Propagate the count increase up to the root of the tree
		while (curNodeIndex != rootNodeIndex)
		{
			// Find the block leader of the block
			// Note: the block leader is the "rightmost" node with count equal to the
			//  count of the current node, BEFORE the count of the current node is
			//  updated. (The current node has already had it's count updated.)
			NodeIndex blockLeaderIndex = curNodeIndex;
			while (subtreeCount[curNodeIndex] > subtreeCount[blockLeaderIndex + 1])
				blockLeaderIndex++;

			// Check if Current Node needs to be swapped with the Block Leader
			//if (curNodeIndex != blockLeaderIndex)
			{
				SwapNodes(curNodeIndex, blockLeaderIndex);
				curNodeIndex = blockLeaderIndex;	// Update index of current node
			}

			// Follow the current node up to it's parent
			curNodeIndex = parentIndex[curNodeIndex];
			// Increment the count of this new node
			subtreeCount[curNodeIndex]++;
		}
	}



	// Raise exception if nodeIndex is out of range
	void AdaptiveHuffmanTree::VerifyNodeIndexInBounds(NodeIndex nodeIndex)
	{
		if (nodeIndex >= nodeCount)
		{
			throw std::runtime_error("AdaptiveHuffmanTree NodeIndex of " + std::to_string(nodeIndex)
				+ " is out of range " + std::to_string(nodeCount));
		}
	}

	// Raise exception if code is out of range
	void AdaptiveHuffmanTree::VerifyNodeDataInBounds(NodeData code)
	{
		if (code >= terminalNodeCount)
		{
			throw std::runtime_error("AdaptiveHuffmanTree NodeData of " + std::to_string(code)
				+ " is out of range " + std::to_string(terminalNodeCount));
		}
	}

	// Private function to swap two nodes in the Huffman tree.
	// This is used during tree restructing by UpdateCodeCount.
	void AdaptiveHuffmanTree::SwapNodes(NodeIndex nodeIndex1, NodeIndex nodeIndex2)
	{
		// Swap Count values
		std::swap(subtreeCount[nodeIndex1], subtreeCount[nodeIndex2]);

		// Update the Parent of the children
		// Note: If the current node is a terminal node (data node) then the left
		//  child is a code value and it's "parent" must be updated but there is
		//  no right child to update a parent link for. If the current node is not
		//  a terminal node (not a data node) then both left and right child nodes
		//  need to have their parent link updated

		auto temp = linkOrData[nodeIndex1];
		parentIndex[temp] = nodeIndex2;			// Update left child
		if (temp < nodeCount)			// Check for non-data node (has right child)
			parentIndex[temp + 1] = nodeIndex2;	// Update right child

		temp = linkOrData[nodeIndex2];
		parentIndex[temp] = nodeIndex1;			// Update left child
		if (temp < nodeCount)			// Check for non-data node (has right child)
			parentIndex[temp + 1] = nodeIndex1;	// Update right child

		// Swap Data values (link to children or code value)
		std::swap(linkOrData[nodeIndex1], linkOrData[nodeIndex2]);
	}



	// Used during compression to get the bitstring to emit for a given code.
	// Returns the bitstring for a given code
	// Places the length of the bitstring in the bitCount out parameter
	// NOTE: Bit order subject to change (and likely will change). Currently:
	// The branch to take between the root and a child of the root is placed in the LSB
	// Subsequent branches are stored in higher bits
	unsigned int AdaptiveHuffmanTree::GetEncodedBitString(NodeData code, unsigned int& bitCount)
	{
		VerifyNodeDataInBounds(code);

		// Record the path to the root
		bitCount = 0;
		unsigned int bitString = 0;
		NodeIndex curNodeIndex = code;
		while (curNodeIndex != rootNodeIndex)
		{
			unsigned int bBit = curNodeIndex & 1;  // Get the direction from parent to current node
			bitString = (bitString << 1) | bBit;  // Pack the bit into the returned string
			bitCount++;
			curNodeIndex = parentIndex[curNodeIndex];
		}

		return bitString;
	}
}
