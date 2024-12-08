#include "Node.hpp"
#include <stdexcept>

namespace sbd
{
  Node::Node() : pointers(), size(0), isLeaf(true), parentIndex(-1)
  {
    pointers.fill(-1);
    for (auto &record : records)
    {
      record = std::make_tuple(-1, -1);
    }
  }

  Node::~Node()
  {
  }

  void Node::setKey(std::int32_t key, std::int32_t index)
  {
    if (index >= size || index < 0)
      throw std::runtime_error("Index out of bounds");
    records[index] = std::make_tuple(key, std::get<1>(records[index]));
  }

  void Node::setAddress(std::int32_t address, std::int32_t index)
  {
    if (index >= size || index < 0)
      throw std::runtime_error("Index out of bounds");
    records[index] = std::make_tuple(std::get<0>(records[index]), address);
  }

  void Node::setPointer(std::int32_t pointer, std::int32_t index)
  {
    if (index >= size + 1 || index < 0)
      throw std::runtime_error("Index out of bounds");
    pointers[index] = pointer;
  }

  void Node::setRecord(std::tuple<std::int32_t, std::int32_t> record, std::int32_t index)
  {
    if (index >= size || index < 0)
      throw std::runtime_error("Index out of bounds");
    records[index] = record;
  }

  std::int32_t Node::getKey(std::int32_t index)
  {
    if (index >= MAX_RECORDS)
      throw std::runtime_error("Index out of bounds");
    return std::get<0>(records[index]);
  }

  std::int32_t Node::getAddress(std::int32_t index)
  {
    if (index >= MAX_RECORDS)
      throw std::runtime_error("Index out of bounds");
    return std::get<1>(records[index]);
  }

  std::int32_t Node::getPointer(std::int32_t index)
  {
    if (index >= MAX_POINTERS)
      throw std::runtime_error("Index out of bounds");
    return pointers[index];
  }

  std::tuple<std::int32_t, std::int32_t> Node::getRecord(std::int32_t index)
  {
    if (index >= MAX_RECORDS)
      throw std::runtime_error("Index out of bounds");
    return records[index];
  }
}