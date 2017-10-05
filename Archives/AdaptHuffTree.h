namespace Archives
{
	// The Huffman Tree stores codes in the terminal nodes of the (binary) tree.
	// The tree is initially balanced with codes 0 to numTerminalNodes-1 in the
	// terminal nodes at the bottom of the tree. The tree branches are traversed
	// using binary values: "0" traverse Left branch, "1" traverse Right branch.
	class AdaptHuffTree
	{
	public:
		typedef unsigned short USHORT;

		AdaptHuffTree(USHORT numTerminalNodes);
		~AdaptHuffTree();

		// Decompression routines
		USHORT GetRootNodeIndex();				// Get root of tree to start search at
		USHORT GetChildNode(USHORT nodeIndex, bool bRight);
		// bRight = 1 --> follow right branch
		bool IsLeaf(USHORT nodeIndex);			// Determines if node is terminal node
		USHORT GetNodeData(USHORT nodeIndex);	// Returns the data in a terminal node

		// Tree restructuring routines
		void UpdateCodeCount(USHORT code);		// Perform tree update/restructure

		// Compression routines
		int GetEncodedBitString(int code, int &bitString);
		// Retuns the length of the path from the
		// root node to the node with the given
		// code and places the path in bitString

	private:
		void SwapNodes(USHORT node1, USHORT node2);

		// Next three arrays comprise the adaptive huffman tree
		// Note: m_Data is used for both tree links and node data.
		//   Values above or equal m_NumNodes represent data
		//   Values below m_NumNodes represent links
		USHORT *m_Data;		// index of left child (link) or code+m_NumNodes (data)
		USHORT *m_Count;	// number of occurances of code or codes in subtrees
		USHORT *m_Parent;	// index of the parent node to (current node or data)
							//  Note: This is also used to translate: code -> node_index
		// Tree properties
		USHORT m_RootNodeIndex;
		USHORT m_NumTerminalNodes;
		USHORT m_NumNodes;
	};




	// Implementation Notes
	// --------------------

	// Note: the m_Data array contains both link data and code data. If the value is
	//  less than m_NumNodes then the value represents the index of the node which is
	//  the left child of the current node and value+1 represents the index of the
	//  node which is the right child of the current node.
	//  If the value is greater or equal to m_NumNodes then the current node is a
	//  terminal node and the code stored in this node is value-m_NumNodes.

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

	// Note: The m_Parent array contains more entries than the other two. Entries below
	//  m_NumNodes are used to find the parent of a given node. Entries above m_NumNodes
	//  (up to m_NumNodes + m_NumTerminalNodes) are used to find the node which contains
	//  a given code. i.e. the "parent" of the code.

	// Note: Parents are always to the "right" of their children. Also every node except
	//  the root has a sibling. Nodes with a higher count value are always found further
	//  to the right in the tree. Thus the tree maintains the Sibling Property.
}