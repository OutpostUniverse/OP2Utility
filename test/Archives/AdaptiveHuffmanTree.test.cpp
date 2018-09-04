#include "Archives/AdaptiveHuffmanTree.h"
#include <gtest/gtest.h>


TEST(AdaptiveHuffmanTreeTests, SimpleTree2) {
	// Construct minimum non-degenerate binary tree with 2 data nodes
	Archives::AdaptiveHuffmanTree tree(2);

	// With 2 data nodes, the root can not be a data node
	auto rootIndex = tree.GetRootNodeIndex();
	ASSERT_FALSE(tree.IsLeaf(rootIndex));

	// With 2 data nodes, the children of the root must be data nodes
	auto leftChild = tree.GetChildNode(rootIndex, false);
	auto rightChild = tree.GetChildNode(rootIndex, true);
	ASSERT_TRUE(tree.IsLeaf(leftChild));
	ASSERT_TRUE(tree.IsLeaf(rightChild));

	// No tree updates, so initial data should still be in order
	ASSERT_EQ(0, tree.GetNodeData(leftChild));
	ASSERT_EQ(1, tree.GetNodeData(rightChild));
}


class AdaptiveHuffmanTreeOutpost2 : public ::testing::Test {
protected:
	Archives::AdaptiveHuffmanTree tree{314};
};

// Test the encoder and decoder against each other
TEST_F(AdaptiveHuffmanTreeOutpost2, EncodeDecode) {
	auto codeCount = tree.TerminalNodeCount();
	for (unsigned int i = 0; i < codeCount; ++i) {
		unsigned int codeLength;
		auto bitString = tree.GetEncodedBitString(i, codeLength);
		auto node = tree.GetRootNodeIndex();
		for (; codeLength > 0; --codeLength) {
			ASSERT_FALSE(tree.IsLeaf(node));
			node = tree.GetChildNode(node, bitString & 0x01);
			bitString >>= 1;
		}
		ASSERT_TRUE(tree.IsLeaf(node));
		ASSERT_EQ(i, tree.GetNodeData(node));
	}
}
