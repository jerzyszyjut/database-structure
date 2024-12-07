#include <array>
#include <cstdint>
#include "constants.hpp"
#pragma once

namespace sbd
{
  class Node
  {
  public:
    std::array<std::int32_t, MAX_RECORDS> keys;
    std::array<std::int32_t, MAX_RECORDS> addresses;
    std::array<std::int32_t, MAX_POINTERS> pointers;
    std::int32_t size;
    std::int32_t parentIndex;
    bool isLeaf;

    Node();
    ~Node();
    // Node &operator=(const Node &other);
    // Node(const Node &other);
    // Node &operator=(Node &&other);
    // Node(Node &&other);
  };
} // namespace sbd