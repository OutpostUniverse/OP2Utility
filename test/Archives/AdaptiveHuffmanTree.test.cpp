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
