#include <array>
#include <tuple>
#include <cstdint>
#include "constants.hpp"
#pragma once

namespace sbd
{
  class Node
  {
  private:
    std::array<std::tuple<std::int32_t, std::int32_t>, MAX_RECORDS> records;
    std::array<std::int32_t, MAX_POINTERS> pointers;

  public:
    std::int32_t size;
    std::int32_t parentIndex;
    bool isLeaf;

    Node();
    ~Node();
    void setKey(std::int32_t key, std::int32_t index);
    void setAddress(std::int32_t address, std::int32_t index);
    void setPointer(std::int32_t pointer, std::int32_t index);
    void setRecord(std::tuple<std::int32_t, std::int32_t> record, std::int32_t index);
    std::int32_t getKey(std::int32_t index);
    std::int32_t getAddress(std::int32_t index);
    std::int32_t getPointer(std::int32_t index);
    std::tuple<std::int32_t, std::int32_t> getRecord(std::int32_t index);
    // Node &operator=(const Node &other);
    // Node(const Node &other);
    // Node &operator=(Node &&other);
    // Node(Node &&other);
  };
} // namespace sbd