#include <vector>
#include <fstream>
#include "Node.hpp"

namespace sbd
{
  class Tree
  {
  private:
    std::vector<Node> nodes;
    std::int32_t rootIndex;

  public:
    Tree();
    ~Tree();
    std::int32_t search(std::int32_t key);
    std::int32_t search(std::int32_t nodeIndex, std::int32_t key);
    void insert(std::int32_t key, std::int32_t address);
    void insertNonFull(std::int32_t nodeIndex, std::int32_t key, std::int32_t address);
    void splitChild(std::int32_t rootIndex, std::int32_t split, std::int32_t childIndex);
    void remove(std::int32_t key);
    void createDotFile(std::string filename);
    void createDotFile(std::ofstream &file, std::int32_t nodeIndex);
  };
} // namespace sbd