#include "Tree.hpp"
#include "Node.hpp"
#include <string>

namespace sbd
{
  Tree::Tree() : nodes(), rootIndex(0)
  {
    nodes.push_back(sbd::Node());
    nodes[rootIndex].isLeaf = true;
  }

  Tree::~Tree()
  {
  }

  std::int32_t Tree::search(std::int32_t key)
  {
    if (nodes[rootIndex].size == 0)
      return -1;
    return search(rootIndex, key);
  }

  std::int32_t Tree::search(std::int32_t nodeIndex, std::int32_t key)
  {
    std::int32_t i = 0;
    sbd::Node &node = nodes[nodeIndex];

    while (i < node.size && key > node.getKey(i))
      ++i;

    if (i < node.size && key == node.getKey(i))
      return node.getAddress(i);
    else if (node.isLeaf)
      return -1;
    else
      return search(node.getPointer(i), key);
  }

  std::int32_t Tree::insert(std::int32_t key, std::int32_t address)
  {
    return insert(rootIndex, key, address);
  }

  std::int32_t Tree::insert(std::int32_t nodeIndex, std::int32_t key, std::int32_t address)
  {
    sbd::Node &node = nodes[nodeIndex];
    std::int32_t i = 0;

    while (i < node.size && key > node.getKey(i))
    {
      ++i;
      if (i < node.size && key == node.getKey(i))
        return ALREADY_EXISTS;
    }

    if (node.isLeaf)
    {
      if (node.size < MAX_RECORDS)
      {
        node.size++;
        for (auto j = node.size - 1; j > i; --j)
        {
          node.setRecord(node.getRecord(j - 1), j);
        }
        node.setRecord(std::make_tuple(key, address), i);
      }

      if (node.size >= MAX_RECORDS)
      {
        bool compensated = tryCompensate(nodeIndex, key);
        if (!compensated)
          split(nodeIndex);
      }
      return OK;
    }
    else
    {
      std::int32_t childIndex = node.getPointer(i);
      return insert(childIndex, key, address);
    }
  }

  bool Tree::tryCompensate(std::int32_t mainIndex, std::int32_t key)
  {
    sbd::Node &main = nodes[mainIndex];
    if (main.parentIndex == -1)
      return false;

    sbd::Node &parent = nodes[main.parentIndex];

    std::int32_t i = 1;
    while (i < parent.size && key > parent.getKey(i))
      ++i;

    std::int32_t leftNeighborIndex = (i > 0) ? parent.getPointer(i - 1) : -1;
    std::int32_t rightNeighborIndex = (i <= parent.size) ? parent.getPointer(i) : -1;
    leftNeighborIndex = (leftNeighborIndex != -1 && nodes[leftNeighborIndex].size < MAX_RECORDS - 1) ? leftNeighborIndex : -1;
    rightNeighborIndex = (rightNeighborIndex != -1 && nodes[rightNeighborIndex].size < MAX_RECORDS - 1) ? rightNeighborIndex : -1;

    if (leftNeighborIndex != -1 || rightNeighborIndex != -1)
    {
      sbd::Node &neighbor = (leftNeighborIndex != -1) ? nodes[leftNeighborIndex] : nodes[rightNeighborIndex];
      sbd::Node &left = (leftNeighborIndex != -1) ? nodes[leftNeighborIndex] : main;
      sbd::Node &right = (leftNeighborIndex != -1) ? main : nodes[rightNeighborIndex];

      std::array<std::tuple<std::int32_t, std::int32_t>, MAX_RECORDS * 2> allRecords;
      std::array<std::int32_t, MAX_POINTERS * 2> allPointers;
      std::int32_t allSize = 0;

      for (auto j = 0; j < left.size; ++j)
      {
        allRecords[allSize] = left.getRecord(j);
        allPointers[allSize] = left.getPointer(j);
        allSize++;
      }
      allPointers[allSize] = left.getPointer(left.size);

      allRecords[allSize] = parent.getRecord(i - 1);
      allSize++;

      for (auto j = 0; j < right.size; ++j)
      {
        allRecords[allSize] = right.getRecord(j);
        allPointers[allSize] = right.getPointer(j);
        allSize++;
      }

      allPointers[allSize] = right.getPointer(right.size);

      std::int32_t split = allSize / 2;

      // leftNeighborIndex = (leftNeighborIndex != -1) ? leftNeighborIndex : mainIndex;
      // rightNeighborIndex = (rightNeighborIndex != -1) ? rightNeighborIndex : mainIndex;
      // sbd::Node &left = nodes[leftNeighborIndex];
      // sbd::Node &right = nodes[rightNeighborIndex];

      left.size = 0;
      for (auto j = 0; j < split; ++j)
      {
        left.size++;
        left.setRecord(allRecords[j], j);
        left.setPointer(allPointers[j], j);
      }
      left.setPointer(allPointers[split], left.size);

      parent.setRecord(allRecords[split], i - 1);
      // parent.setPointer(leftNeighborIndex, i - 1);
      // parent.setPointer(rightNeighborIndex, i);

      right.size = 0;
      for (auto j = split + 1; j < allSize; ++j)
      {
        right.size++;
        right.setRecord(allRecords[j], right.size - 1);
        right.setPointer(allPointers[j], right.size - 1);
      }
      right.setPointer(allPointers[allSize], right.size);

      return true;
    }
    return false;
  }

  void Tree::split(std::int32_t nodeIndex)
  {
    sbd::Node &node = nodes[nodeIndex];
    std::int32_t midIndex = node.size / 2;

    sbd::Node newNode;
    newNode.isLeaf = node.isLeaf;
    newNode.parentIndex = node.parentIndex;

    for (std::int32_t i = midIndex + 1, j = 0; i < node.size; ++i, ++j)
    {
      newNode.size++;
      newNode.setRecord(node.getRecord(i), j);
      if (!node.isLeaf)
      {
        newNode.setPointer(node.getPointer(i), j);
        nodes[node.getPointer(i)].parentIndex = nodes.size();
      }
    }

    if (!node.isLeaf)
    {
      newNode.setPointer(node.getPointer(node.size), newNode.size);
      nodes[node.getPointer(node.size)].parentIndex = nodes.size();
    }

    node.size = midIndex;

    nodes.push_back(newNode);
    std::int32_t newNodeIndex = nodes.size() - 1;
    node = nodes[nodeIndex];

    if (node.parentIndex == -1)
    {
      sbd::Node newRoot;
      newRoot.isLeaf = false;
      newRoot.size = 1;
      newRoot.setRecord(node.getRecord(midIndex), 0);
      newRoot.setPointer(nodeIndex, 0);
      newRoot.setPointer(newNodeIndex, 1);

      nodes[nodeIndex].parentIndex = nodes.size();
      nodes[newNodeIndex].parentIndex = nodes.size();

      nodes.push_back(newRoot);
      rootIndex = nodes.size() - 1;
    }
    else
    {
      sbd::Node &parent = nodes[node.parentIndex];
      std::int32_t i = 0;

      while (i < parent.size && node.getKey(midIndex) > parent.getKey(i))
        ++i;

      parent.size++;
      for (std::int32_t j = parent.size - 1; j > i; --j)
      {
        parent.setRecord(parent.getRecord(j - 1), j);
        parent.setPointer(parent.getPointer(j), j + 1);
      }
      parent.setRecord(node.getRecord(midIndex), i);
      parent.setPointer(newNodeIndex, i + 1);

      if (parent.size >= MAX_RECORDS)
      {
        bool compensated = tryCompensate(node.parentIndex, parent.getKey(i));
        if (!compensated)
          split(node.parentIndex);
      }
    }
  }

  void Tree::createDotFile(std::string filename)
  {
    std::ofstream file(filename);
    file << "digraph G {" << std::endl;
    file << "node [shape=record];" << std::endl;

    for (auto i = 0; i < nodes.size(); ++i)
    {
      auto &node = nodes[i];
      file << "node" << i << "[label=\"";
      for (auto i = 0; i < node.size; ++i)
      {
        file << "<f" << i << "> |" << node.getKey(i) << "| ";
      }
      file << "<f" << node.size << ">\"";
      if (node.isLeaf)
      {
        file << ", style=filled, fillcolor=lightblue";
      }
      if (node.parentIndex == -1)
      {
        file << ", style=filled, fillcolor=lightgreen";
      }
      file << "];" << std::endl;
    }

    createDotFile(file, rootIndex);
    file << "}" << std::endl;
    file.close();
  }

  void Tree::createDotFile(std::ofstream &file, std::int32_t nodeIndex)
  {
    auto &node = nodes[nodeIndex];
    if (!node.isLeaf)
    {
      for (auto i = 0; i <= node.size; ++i)
      {
        if (node.getPointer(i) != -1)
        {
          file << "\"node" << nodeIndex << "\":f" << i << " -> \"node" << node.getPointer(i) << "\"" << std::endl;
          createDotFile(file, node.getPointer(i));
        }
      }
    }
    // file << "\"node" << nodeIndex << "\"" << " -> \"node" << node.parentIndex << "\"" << std::endl;
  }

} // namespace sbd
