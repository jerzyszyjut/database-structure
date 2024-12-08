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
    std::vector<Node> nodes;
    std::int32_t rootIndex;
    std::ofstream file;

  public:
    Tree();
    ~Tree();
    std::int32_t search(std::int32_t key);
    std::int32_t search(std::int32_t nodeIndex, std::int32_t key);
    std::int32_t insert(std::int32_t key, std::int32_t address);
    std::int32_t insert(std::int32_t nodeIndex, std::int32_t key, std::int32_t address);
    sbd::Node &getNode(std::int32_t index);
    bool tryCompensate(std::int32_t mainIndex, std::int32_t split);
    void split(std::int32_t nodeIndex);
    void remove(std::int32_t key);
    void createDotFile(std::string filename);
    void createDotFile(std::ofstream &file, std::int32_t nodeIndex);
  };
} // namespace sbd