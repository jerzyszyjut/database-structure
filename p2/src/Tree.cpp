#include "Tree.hpp"
#include "Node.hpp"
#include "Counter.hpp"
#include <string>

namespace sbd
{
  void Tree::saveNodeToFile(sbd::Node node)
  {
    treeFile.seekp(node.getId() * sbd::NODE_SIZE);
    std::array<char, sbd::NODE_SIZE> bytes = node.toBytes();
    treeFile.write(bytes.data(), sbd::NODE_SIZE);
    sbd::Counters::getInstance().incrementWrite();
    if (treeFile.fail())
    {
      throw std::runtime_error("Error writing to file");
    }
  }

  sbd::Node Tree::readNodeFromFile(std::int32_t index)
  {
    treeFile.seekg(index * sbd::NODE_SIZE);
    std::array<char, sbd::NODE_SIZE> bytes;
    treeFile.read(bytes.data(), sbd::NODE_SIZE);
    sbd::Node node;
    node.fromBytes(bytes);
    sbd::Counters::getInstance().incrementRead();
    return node;
  }

  Tree::Tree() : rootIndex(0)
  {
    treeFile.open(sbd::TREE_FILE_NAME, std::fstream::in | std::fstream::out | std::fstream::binary | std::fstream::trunc);
    sbd::Node root = sbd::Node();
    root.setParentIndex(-1);
    root.setIsLeaf(true);
    root.setSize(0);
    updateNode(root);
  }

  Tree::~Tree()
  {
    if (treeFile.is_open())
    {
      treeFile.close();
    }
  }

  std::int32_t Tree::search(float key)
  {
    if (getNode(rootIndex).getSize() == 0)
      return -1;
    return search(rootIndex, key);
  }

  std::int32_t Tree::search(std::int32_t nodeIndex, float key)
  {
    std::int32_t i = 0;
    sbd::Node node = getNode(nodeIndex);

    while (i < node.getSize() && key > node.getKey(i))
      ++i;

    if (i < node.getSize() && key == node.getKey(i))
      return node.getAddress(i);
    else if (node.getIsLeaf())
      return -1;
    else
      return search(node.getPointer(i), key);
  }

  std::int32_t Tree::insert(float key, std::int32_t address)
  {
    return insert(rootIndex, key, address);
  }

  std::int32_t Tree::insert(std::int32_t nodeIndex, float key, std::int32_t address)
  {
    sbd::Node node = getNode(nodeIndex);
    std::int32_t i = 0;

    while (i < node.getSize() && key > node.getKey(i))
    {
      ++i;
      if (i < node.getSize() && key == node.getKey(i))
        return ALREADY_EXISTS;
    }

    if (node.getIsLeaf())
    {
      if (node.getSize() < MAX_RECORDS)
      {
        node.setSize(node.getSize() + 1);
        for (auto j = node.getSize() - 1; j > i; --j)
        {
          node.setRecord(node.getRecord(j - 1), j);
        }
        node.setRecord(std::make_tuple(key, address), i);
      }

      updateNode(node);

      if (node.getSize() >= MAX_RECORDS)
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

  bool Tree::tryCompensate(std::int32_t mainIndex, float key)
  {
    sbd::Node main = getNode(mainIndex);

    if (main.getParentIndex() == -1)
      return false;

    sbd::Node parent = getNode(main.getParentIndex());

    std::int32_t i = 1;
    while (i < parent.getSize() && key > parent.getKey(i))
      ++i;

    std::int32_t leftNeighborIndex = (i > 0) ? parent.getPointer(i - 1) : -1;
    std::int32_t rightNeighborIndex = (i <= parent.getSize()) ? parent.getPointer(i) : -1;
    leftNeighborIndex = (leftNeighborIndex != -1 && getNode(leftNeighborIndex).getSize() < MAX_RECORDS - 1) ? leftNeighborIndex : -1;
    rightNeighborIndex = (rightNeighborIndex != -1 && getNode(rightNeighborIndex).getSize() < MAX_RECORDS - 1) ? rightNeighborIndex : -1;

    if (leftNeighborIndex != -1 || rightNeighborIndex != -1)
    {
      sbd::Node neighbor = (leftNeighborIndex != -1) ? getNode(leftNeighborIndex) : getNode(rightNeighborIndex);
      sbd::Node left = (leftNeighborIndex != -1) ? getNode(leftNeighborIndex) : main;
      sbd::Node right = (leftNeighborIndex != -1) ? main : getNode(rightNeighborIndex);

      std::array<std::tuple<float, std::int32_t>, MAX_RECORDS * 2> allRecords;
      std::array<std::int32_t, MAX_POINTERS * 2> allPointers;
      std::int32_t allSize = 0;

      for (auto j = 0; j < left.getSize(); ++j)
      {
        allRecords[allSize] = left.getRecord(j);
        allPointers[allSize] = left.getPointer(j);
        allSize++;
      }
      allPointers[allSize] = left.getPointer(left.getSize());

      allRecords[allSize] = parent.getRecord(i - 1);
      allSize++;

      for (auto j = 0; j < right.getSize(); ++j)
      {
        allRecords[allSize] = right.getRecord(j);
        allPointers[allSize] = right.getPointer(j);
        allSize++;
      }

      allPointers[allSize] = right.getPointer(right.getSize());

      std::int32_t split = allSize / 2;

      left.setSize(0);
      for (auto j = 0; j < split; ++j)
      {
        left.setSize(left.getSize() + 1);
        left.setRecord(allRecords[j], j);
        left.setPointer(allPointers[j], j);
        if (!left.getIsLeaf())
        {
          sbd::Node child = getNode(allPointers[j]);
          child.setParentIndex(left.getId());
          updateNode(child);
        }
      }
      if (!left.getIsLeaf())
      {
        sbd::Node child = getNode(allPointers[split]);
        child.setParentIndex(left.getId());
        updateNode(child);
      }
      left.setPointer(allPointers[split], left.getSize());

      parent.setRecord(allRecords[split], i - 1);
      right.setSize(0);
      for (auto j = split + 1; j < allSize; ++j)
      {
        right.setSize(right.getSize() + 1);
        right.setRecord(allRecords[j], right.getSize() - 1);
        right.setPointer(allPointers[j], right.getSize() - 1);
        if (!right.getIsLeaf())
        {
          sbd::Node child = getNode(allPointers[j]);
          child.setParentIndex(right.getId());
          updateNode(child);
        }
      }
      right.setPointer(allPointers[allSize], right.getSize());
      if (!right.getIsLeaf())
      {
        sbd::Node child = getNode(allPointers[allSize]);
        child.setParentIndex(right.getId());
        updateNode(child);
      }

      updateNode(left);
      updateNode(right);
      updateNode(parent);

      return true;
    }
    return false;
  }

  void Tree::split(std::int32_t nodeIndex)
  {
    sbd::Node node = getNode(nodeIndex);
    std::int32_t midIndex = node.getSize() / 2;

    sbd::Node newNode;
    newNode.setIsLeaf(node.getIsLeaf());
    newNode.setParentIndex(node.getParentIndex());

    for (std::int32_t i = midIndex + 1, j = 0; i < node.getSize(); ++i, ++j)
    {
      newNode.setSize(newNode.getSize() + 1);
      newNode.setRecord(node.getRecord(i), j);
      if (!node.getIsLeaf())
      {
        newNode.setPointer(node.getPointer(i), j);
        sbd::Node child = getNode(node.getPointer(i));
        child.setParentIndex(getNextNodeIndex());
        updateNode(child);
      }
    }

    if (!node.getIsLeaf())
    {
      newNode.setPointer(node.getPointer(node.getSize()), newNode.getSize());
      sbd::Node last = getNode(node.getPointer(node.getSize()));
      last.setParentIndex(getNextNodeIndex());
      updateNode(last);
    }

    node.setSize(midIndex);

    updateNode(node);

    std::int32_t newNodeIndex = updateNode(newNode);

    if (node.getParentIndex() == -1)
    {
      sbd::Node newRoot;
      newRoot.setIsLeaf(false);
      newRoot.setSize(1);
      newRoot.setRecord(node.getRecord(midIndex), 0);
      newRoot.setPointer(nodeIndex, 0);
      newRoot.setPointer(newNodeIndex, 1);

      rootIndex = updateNode(newRoot);

      node.setParentIndex(rootIndex);
      newNode.setParentIndex(rootIndex);

      updateNode(node);
      updateNode(newNode);
    }
    else
    {
      sbd::Node parent = getNode(node.getParentIndex());
      std::int32_t i = 0;

      while (i < parent.getSize() && node.getKey(midIndex) > parent.getKey(i))
        ++i;

      parent.setSize(parent.getSize() + 1);
      for (std::int32_t j = parent.getSize() - 1; j > i; --j)
      {
        parent.setRecord(parent.getRecord(j - 1), j);
        parent.setPointer(parent.getPointer(j), j + 1);
      }
      parent.setRecord(node.getRecord(midIndex), i);
      parent.setPointer(newNodeIndex, i + 1);

      updateNode(parent);

      if (parent.getSize() >= MAX_RECORDS)
      {
        bool compensated = tryCompensate(node.getParentIndex(), parent.getKey(i));
        if (!compensated)
          split(node.getParentIndex());
      }
    }
  }

  sbd::Node Tree::getNode(std::int32_t index)
  {
    if (index >= currentIndex || index < 0)
      throw std::runtime_error("Index out of bounds");

    if (index == rootIndex)
      return rootNode;

    sbd::Node node = readNodeFromFile(index);
    node.setId(index, false);
    return node;
    // return nodes[index];
  }

  std::int32_t Tree::updateNode(sbd::Node &node)
  {
    if (!node.isDirty())
      return node.getId();

    node.cleanNode();

    if (node.getId() == -1)
    {
      node.setId(getNextNodeIndex());
      currentIndex++;
    }

    node.cleanNode();

    if (node.getParentIndex() == -1)
      rootNode = node;

    saveNodeToFile(node);
    // nodes.push_back(node);
    return node.getId();
  }

  std::int32_t Tree::getNextNodeIndex()
  {
    return currentIndex;
    // return nodes.size();
  }

  void Tree::createDotFile(std::string filename)
  {
    std::ofstream file(filename);
    file << "digraph G {" << std::endl;
    file << "node [shape=record];" << std::endl;

    for (auto i = 0; i < currentIndex; ++i)
    {
      auto node = getNode(i);
      file << "node" << i << "[label=\"";
      for (auto i = 0; i < node.getSize(); ++i)
      {
        file << "<f" << i << "> |" << node.getKey(i) << "| ";
      }
      file << "<f" << node.getSize() << ">\"";
      if (node.getIsLeaf())
      {
        file << ", style=filled, fillcolor=lightblue";
      }
      if (node.getParentIndex() == -1)
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
    auto node = getNode(nodeIndex);
    if (!node.getIsLeaf())
    {
      for (auto i = 0; i <= node.getSize(); ++i)
      {
        if (node.getPointer(i) != -1)
        {
          file << "\"node" << nodeIndex << "\":f" << i << " -> \"node" << node.getPointer(i) << "\"" << std::endl;
          createDotFile(file, node.getPointer(i));
        }
      }
    }
    // file << "\"node" << nodeIndex << "\"" << " -> \"node" << node.getParentIndex() << "\"" << std::endl;
  }

} // namespace sbd
