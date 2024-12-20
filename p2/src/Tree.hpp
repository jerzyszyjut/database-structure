#pragma once
#include <vector>
#include <fstream>
#include "Node.hpp"

#define OK 0
#define ALREADY_EXISTS 1

namespace sbd
{
  class Tree
  {
  private:
    std::int32_t rootIndex;
    sbd::Node rootNode;
    std::fstream treeFile;
    std::int32_t currentIndex = 0;
    void saveNodeToFile(sbd::Node node);
    sbd::Node readNodeFromFile(std::int32_t index);

  public:
    Tree();
    ~Tree();
    std::int32_t search(float key);
    std::int32_t search(std::int32_t nodeIndex, float key);
    std::int32_t insert(float key, std::int32_t address);
    std::int32_t insert(std::int32_t nodeIndex, float key, std::int32_t address);
    bool tryCompensate(std::int32_t mainIndex, float key);
    void split(std::int32_t nodeIndex);
    sbd::Node getNode(std::int32_t index);
    std::int32_t updateNode(sbd::Node &node);
    std::int32_t getNextNodeIndex();
    void createDotFile(std::string filename);
    void createDotFile(std::ofstream &file, std::int32_t nodeIndex);
    std::int32_t getHeight();
    std::int32_t getHeight(sbd::Node node);
    float getMemoryUsageRatio();
    std::tuple<std::int32_t, std::int32_t> getMemoryUsageRatio(sbd::Node node);
  };
} // namespace sbd