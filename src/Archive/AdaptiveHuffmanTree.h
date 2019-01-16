#include <vector>

namespace Archive
{
	// The Huffman Tree stores codes in the terminal nodes of the (binary) tree.
	// The tree is initially balanced with codes 0 to terminalNodeCount-1 in the
	// terminal nodes at the bottom of the tree. The tree branches are traversed
	// using binary values: "0" traverse Left branch, "1" traverse Right branch.
	class AdaptiveHuffmanTree
	{
	public:
		using NodeType = unsigned short;
		using NodeIndex = NodeType;
		using NodeData = NodeType;

		AdaptiveHuffmanTree(NodeType terminalNodeCount);

		// Tree info
		NodeType TerminalNodeCount();

		// Decompression routines
		NodeIndex GetRootNodeIndex();				// Get root of tree to start search at
		NodeIndex GetChildNode(NodeIndex nodeIndex, bool bRight);
		// bRight = 1 --> follow right branch
		bool IsLeaf(NodeIndex nodeIndex);			// Determines if node is terminal node
		NodeData GetNodeData(NodeIndex nodeIndex);	// Returns the data in a terminal node

		// Tree restructuring routines
		void UpdateCodeCount(NodeData code);		// Perform tree update/restructure

		// Compression routines
		// NOTE: experimental, API likely to change
		// Retuns the path from the root node to the node with the given code
		// Places the path length in bitCount
		unsigned int GetEncodedBitString(NodeData code, unsigned int &bitCount);

	private:
		void VerifyNodeIndexInBounds(NodeIndex nodeIndex);
		void VerifyNodeDataInBounds(NodeData code);
		void SwapNodes(NodeIndex nodeIndex1, NodeIndex nodeIndex2);

		// Tree properties
		NodeType terminalNodeCount;
		NodeType nodeCount;
		NodeIndex rootNodeIndex;
		// Next three arrays comprise the adaptive huffman tree
		// Note:  is used for both tree links and node data.
		//   Values above or equal nodeCount represent data
		//   Values below nodeCount represent links
		std::vector<NodeType> linkOrData;   // index of left child (link) or code+nodeCount (data)
		std::vector<NodeType> subtreeCount; // number of occurances of code or codes in subtrees
		std::vector<NodeType> parentIndex; // index of the parent node to (current node or data)
							//  Note: This is also used to translate: code -> node_index
	};




	// Implementation Notes
	// --------------------

	// Note: the linkOrData array contains both link data and code data. If the value is
	//  less than nodeCount then the value represents the index of the node which is
	//  the left child of the current node and value+1 represents the index of the
	//  node which is the right child of the current node.
	//  If the value is greater or equal to nodeCount then the current node is a
	//  terminal node and the code stored in this node is value-nodeCount.

	// Note: Block Leader referse to the "rightmost" node whose count is equal to
	//  the count of the current node (before the count of the current node is
	//  updated). When restructing the tree, the current node must be swapped with
	//  the block leader before the count increase is propagated up the tree. The
	//  node to the right is considered the node with the next highest index.

	// Note: Since the tree is fully formed upon initialization and all terminal nodes
	//  have a count of at least 1, there is never a case in which the block leader
	//  is the parent of the current node (since the parent's count must be at least
	//  1 greater than either of it's two subtress, which both exist). Thus such a check
	//  does not need to be performed when swapping the current node with the block leader.

	// Note: The parentIndex array contains more entries than the other two. Entries below
	//  nodeCount are used to find the parent of a given node. Entries above nodeCount
	//  (up to nodeCount + terminalNodeCount) are used to find the node which contains
	//  a given code. i.e. the "parent" of the code.

	// Note: Parents are always to the "right" of their children. Also every node except
	//  the root has a sibling. Nodes with a higher count value are always found further
	//  to the right in the tree. Thus the tree maintains the Sibling Property.
}
