#include "AdaptiveHuffmanTree.h"
#include <utility>
#include <stdexcept>

namespace Archives
{
	// Creates an (adaptive) Huffman tree with numTerminalNodes at the bottom.
	AdaptiveHuffmanTree::AdaptiveHuffmanTree(NodeType numTerminalNodes)
	{
		// Initialize tree properties
		m_NumTerminalNodes = numTerminalNodes;
		m_NumNodes = m_NumTerminalNodes * 2 - 1;
		m_RootNodeIndex = m_NumNodes - 1;

		// Allocate space for the tree
		m_Data = new NodeType[m_NumNodes];
		m_Count = new NodeType[m_NumNodes];
		m_Parent = new NodeType[m_NumNodes + m_NumTerminalNodes];

		// Initialize the tree
		// Initialize terminal nodes
		for (NodeIndex i = 0; i < m_NumTerminalNodes; ++i)
		{
			m_Data[i] = i + m_NumNodes;						// Initilize data values
			m_Count[i] = 1;
			m_Parent[i] = (i >> 1) + m_NumTerminalNodes;
			m_Parent[i + m_NumNodes] = i;						// "Parent of code" (node index)
		}
		// Initialize non terminal nodes
		NodeIndex left = 0;
		for (NodeIndex i = m_NumTerminalNodes; i < m_NumNodes; ++i)
		{
			m_Data[i] = left;								// Initialize link values
			m_Count[i] = m_Count[left] + m_Count[left + 1];	// Count is sum of two subtrees
			m_Parent[i] = (i >> 1) + m_NumTerminalNodes;
			left += 2;										// Calc index of left child (next loop)
		}
	}

	AdaptiveHuffmanTree::~AdaptiveHuffmanTree()
	{
		// Delete the tree
		delete m_Data;
		delete m_Count;
		delete m_Parent;
	}



	// Returns the index of the root node.
	// All tree searches start at the root node.
	AdaptiveHuffmanTree::NodeIndex AdaptiveHuffmanTree::GetRootNodeIndex()
	{
		return m_RootNodeIndex;
	}

	// Return a child node of the given node.
	// This is used to traverse the tree to a terminal node.
	AdaptiveHuffmanTree::NodeIndex AdaptiveHuffmanTree::GetChildNode(NodeIndex nodeIndex, bool bRight)
	{
		// Check that the nodeIndex is in range
		if (nodeIndex >= m_NumNodes)
		{
			throw std::runtime_error("Index out of range");
		}

		// Return the child node index
		return m_Data[nodeIndex] + bRight;
	}

	// Returns true if the node is a terminal node.
	// This is used to know when a tree search has completed.
	bool AdaptiveHuffmanTree::IsLeaf(NodeIndex nodeIndex)
	{
		// Check that the nodeIndex is in range
		if (nodeIndex >= m_NumNodes)
		{
			throw std::runtime_error("Index out of range");
		}

		// Return whether or not this is a terminal node
		return m_Data[nodeIndex] >= m_NumNodes;
	}

	// Returns the data stored in a terminal node
	AdaptiveHuffmanTree::DataValue AdaptiveHuffmanTree::GetNodeData(NodeIndex nodeIndex)
	{
		// Check that the nodeIndex is in range
		if (nodeIndex >= m_NumNodes)
		{
			throw std::runtime_error("Index out of range");
		}

		// Return data stored in node translated back to normal form
		// Note: This assumes the node is a terminal node
		return m_Data[nodeIndex] - m_NumNodes;
	}




	// This updates the count for the given code and restructures the tree if needed.
	// This is used after a tree search to update the tree (gives more frequently used
	// codes a shorter bit encoding).
	void AdaptiveHuffmanTree::UpdateCodeCount(DataValue code)
	{
		int curNodeIndex;
		int blockLeaderIndex;

		// Make sure the code is in range
		if (code >= m_NumTerminalNodes)
		{
			throw std::runtime_error("Code value out of range");
		}

		// Get the index of the node containing this code
		curNodeIndex = m_Parent[code + m_NumNodes];
		m_Count[curNodeIndex]++; // Update the node count

		// Propagate the count increase up to the root of the tree
		while (curNodeIndex != m_RootNodeIndex)
		{
			// Find the block leader of the block
			// Note: the block leader is the "rightmost" node with count equal to the
			//  count of the current node, BEFORE the count of the current node is
			//  updated. (The current node has already had it's count updated.)
			blockLeaderIndex = curNodeIndex;
			while (m_Count[curNodeIndex] > m_Count[blockLeaderIndex + 1])
				blockLeaderIndex++;

			// Check if Current Node needs to be swapped with the Block Leader
			//if (curNodeIndex != blockLeaderIndex)
			{
				SwapNodes(curNodeIndex, blockLeaderIndex);
				curNodeIndex = blockLeaderIndex;	// Update index of current node
			}

			// Follow the current node up to it's parent
			curNodeIndex = m_Parent[curNodeIndex];
			// Increment the count of this new node
			m_Count[curNodeIndex]++;
		}
	}



	// Private function to swap two nodes in the Huffman tree.
	// This is used during tree restructing by UpdateCodeCount.
	void AdaptiveHuffmanTree::SwapNodes(NodeIndex nodeIndex1, NodeIndex nodeIndex2)
	{
		// Swap Count values
		std::swap(m_Count[nodeIndex1], m_Count[nodeIndex2]);

		// Update the Parent of the children
		// Note: If the current node is a terminal node (data node) then the left
		//  child is a code value and it's "parent" must be updated but there is
		//  no right child to update a parent link for. If the current node is not
		//  a terminal node (not a data node) then both left and right child nodes
		//  need to have their parent link updated

		auto temp = m_Data[nodeIndex1];
		m_Parent[temp] = nodeIndex2;			// Update left child
		if (temp < m_NumNodes)			// Check for non-data node (has right child)
			m_Parent[temp + 1] = nodeIndex2;	// Update right child

		temp = m_Data[nodeIndex2];
		m_Parent[temp] = nodeIndex1;			// Update left child
		if (temp < m_NumNodes)			// Check for non-data node (has right child)
			m_Parent[temp + 1] = nodeIndex1;	// Update right child

		// Swap Data values (link to children or code value)
		std::swap(m_Data[nodeIndex1], m_Data[nodeIndex2]);
	}



	// **NOTE**: The following procedure is experimental and is not yet in use.
	//			Nor should it be used yet!
	/*
	// Used during compression to get the bitstring to emit for a given code.
	// The number of bits in the bitstring is returned and the bitstring is placed
	// in the bitString parameter. The branch to take between the root and a child
	// of the root is placed in the LSB. Subsequent branches are stored in higher bits
	// **NOTE**: I may change the bit ordering! (Make that I WILL change the bit ordering)
	int AdaptiveHuffmanTree::GetEncodedBitString(int code, int &bitString)
	{
		int curNodeIndex;
		int numBits;
		bool bBit;

		// Make sure the code is in range
		if (code >= m_NumTerminalNodes)
		{
			throw std::runtime_error("Code value is out of range");
		}

		// Get the node containing the given code
		curNodeIndex = m_Parent[code];

		// Record the path to the root
		bitString = 0;
		while (curNodeIndex != m_RootNodeIndex)
		{
			bBit = curNodeIndex & 0x01;	// Get the direction from parent to current node
			numBits++;
			bitString = (bitString << 1) + bBit;// Pack the bit into the returned string
		}

		return numBits;					// Return number of bits in path from root to node
	}
	*/
}
