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
    std::int32_t id = -1;
    std::array<std::tuple<float, std::int32_t>, MAX_RECORDS> records;
    std::array<std::int32_t, MAX_POINTERS> pointers;
    std::int32_t size;
    std::int32_t parentIndex;
    bool isLeaf;
    bool dirty = true;

  public:
    Node();
    ~Node();
    void setKey(float key, std::int32_t index);
    void setAddress(std::int32_t address, std::int32_t index);
    void setPointer(std::int32_t pointer, std::int32_t index);
    void setRecord(std::tuple<float, std::int32_t> record, std::int32_t index);
    void setId(std::int32_t id);
    void setId(std::int32_t id, bool dirty);
    void setSize(std::int32_t size);
    void setParentIndex(std::int32_t parentIndex);
    void setIsLeaf(bool isLeaf);
    void dirtyNode();
    void cleanNode();
    float getKey(std::int32_t index);
    std::int32_t getAddress(std::int32_t index);
    std::int32_t getPointer(std::int32_t index);
    std::int32_t getSize();
    std::int32_t getParentIndex();
    bool getIsLeaf();
    bool isDirty();
    std::tuple<float, std::int32_t> getRecord(std::int32_t index);
    std::int32_t getId();
    std::array<char, NODE_SIZE> toBytes();
    void fromBytes(std::array<char, NODE_SIZE> bytes);
    Node(const Node &other);
    Node &operator=(const Node &other);
    Node &operator=(Node &&other);
  };
} // namespace sbd