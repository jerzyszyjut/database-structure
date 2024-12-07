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

    while (i < node.size && key > node.keys[i])
      ++i;

    if (i < node.size && key == node.keys[i])
      return node.addresses[i];
    else if (node.isLeaf)
      return -1;
    else
      return search(node.pointers[i], key);
  }

  void Tree::insert(std::int32_t key, std::int32_t address)
  {
    sbd::Node &root = nodes[rootIndex];
    if (root.size == MAX_RECORDS)
    {
      sbd::Node newRoot = sbd::Node();
      newRoot.isLeaf = false;
      newRoot.size = 0;
      newRoot.pointers[0] = rootIndex;
      nodes.push_back(newRoot);
      std::int32_t previousRootIndex = rootIndex;
      rootIndex = nodes.size() - 1;
      splitChild(rootIndex, 0, previousRootIndex);
      insertNonFull(rootIndex, key, address);
    }
    else
    {
      insertNonFull(rootIndex, key, address);
    }
  }

  void Tree::insertNonFull(std::int32_t nodeIndex, std::int32_t key, std::int32_t address)
  {
    sbd::Node &node = nodes[nodeIndex];
    std::int32_t i = node.size - 1;

    if (node.isLeaf)
    {
      while (i >= 0 && key < node.keys[i])
      {
        node.keys[i + 1] = node.keys[i];
        node.addresses[i + 1] = node.addresses[i];
        --i;
      }
      node.keys[i + 1] = key;
      node.addresses[i + 1] = address;
      node.size++;
    }
    else
    {
      while (i >= 0 && key < node.keys[i])
        --i;

      ++i;
      if (nodes[node.pointers[i]].size == MAX_RECORDS)
      {
        splitChild(nodeIndex, i, node.pointers[i]);
        if (key > node.keys[i])
          ++i;
      }
      insertNonFull(node.pointers[i], key, address);
    }
  }

  void Tree::splitChild(std::int32_t mainIndex, std::int32_t split, std::int32_t childIndex)
  {
    sbd::Node &main = nodes[mainIndex];
    sbd::Node &child1 = nodes[childIndex];
    sbd::Node child2 = sbd::Node();

    child2.isLeaf = child1.isLeaf;
    child2.size = MIN_RECORDS - 1;
    for (auto i = 0; i < child2.size; ++i)
    {
      child2.keys[i] = child1.keys[i + MIN_RECORDS + 1];
      child2.addresses[i] = child1.addresses[i + MIN_RECORDS + 1];
    }

    if (!child1.isLeaf)
    {
      for (auto i = 0; i < MIN_POINTERS; ++i)
      {
        child2.pointers[i] = child1.pointers[i + MIN_POINTERS];
      }
    }
    child1.size = MIN_RECORDS;

    for (auto i = main.size; i > split; --i)
    {
      main.pointers[i + 1] = main.pointers[i];
    }

    main.pointers[split + 1] = nodes.size();

    for (auto i = main.size - 1; i >= split; --i)
    {
      main.keys[i + 1] = main.keys[i];
      main.addresses[i + 1] = main.addresses[i];
    }

    main.keys[split] = child1.keys[MIN_RECORDS];
    main.addresses[split] = child1.addresses[MIN_RECORDS];
    main.size++;

    nodes.push_back(child2);
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
        file << "<f" << i << "> |" << node.keys[i] << "| ";
      }
      file << "<f" << node.size << ">\"";
      if (node.isLeaf)
      {
        file << ", style=filled, fillcolor=lightblue";
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
        if (node.pointers[i] != -1)
        {
          file << "\"node" << nodeIndex << "\":f" << i << " -> \"node" << node.pointers[i] << "\"" << std::endl;
          createDotFile(file, node.pointers[i]);
        }
      }
    }
  }

} // namespace sbd
