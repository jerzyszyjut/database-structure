#include "Node.hpp"
#include <stdexcept>
#include <cstring>

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

  void Node::setKey(float key, std::int32_t index)
  {
    if (index >= size || index < 0)
      throw std::runtime_error("Index out of bounds");
    if (std::get<0>(records[index]) == key)
      return;
    dirty = true;
    records[index] = std::make_tuple(key, std::get<1>(records[index]));
  }

  void Node::setAddress(std::int32_t address, std::int32_t index)
  {
    if (index >= size || index < 0)
      throw std::runtime_error("Index out of bounds");
    if (std::get<1>(records[index]) == address)
      return;
    dirty = true;
    records[index] = std::make_tuple(std::get<0>(records[index]), address);
  }

  void Node::setPointer(std::int32_t pointer, std::int32_t index)
  {
    if (index >= size + 1 || index < 0)
      throw std::runtime_error("Index out of bounds");
    if (pointers[index] == pointer)
      return;
    dirty = true;
    pointers[index] = pointer;
  }

  void Node::setRecord(std::tuple<float, std::int32_t> record, std::int32_t index)
  {
    if (index >= size || index < 0)
      throw std::runtime_error("Index out of bounds");
    if (records[index] == record)
      return;
    dirty = true;
    records[index] = record;
  }

  void Node::setId(std::int32_t id)
  {
    if (this->id != -1)
      throw std::runtime_error("Id already set");
    if (id < 0)
      throw std::runtime_error("Id must be positive");
    if (this->id == id)
      return;
    dirty = true;
    this->id = id;
  }

  void Node::setId(std::int32_t id, bool dirty)
  {
    setId(id);
    this->dirty = dirty;
  }

  void Node::setSize(std::int32_t size)
  {
    if (size > MAX_RECORDS || size < 0)
      throw std::runtime_error("Size out of bounds");
    if (this->size == size)
      return;
    dirty = true;
    this->size = size;
  }

  void Node::setParentIndex(std::int32_t parentIndex)
  {
    if (parentIndex < -1)
      throw std::runtime_error("Parent index must be positive");
    if (this->parentIndex == parentIndex)
      return;
    dirty = true;
    this->parentIndex = parentIndex;
  }

  void Node::setIsLeaf(bool isLeaf)
  {
    if (this->isLeaf == isLeaf)
      return;
    dirty = true;
    this->isLeaf = isLeaf;
  }

  void Node::dirtyNode()
  {
    dirty = true;
  }

  void Node::cleanNode()
  {
    dirty = false;
  }

  float Node::getKey(std::int32_t index)
  {
    if (index >= MAX_RECORDS || index < 0)
      throw std::runtime_error("Index out of bounds");
    return std::get<0>(records[index]);
  }

  std::int32_t Node::getAddress(std::int32_t index)
  {
    if (index >= MAX_RECORDS || index < 0)
      throw std::runtime_error("Index out of bounds");
    return std::get<1>(records[index]);
  }

  std::int32_t Node::getPointer(std::int32_t index)
  {
    if (index >= MAX_POINTERS || index < 0)
      throw std::runtime_error("Index out of bounds");
    return pointers[index];
  }

  std::int32_t Node::getSize()
  {
    return size;
  }

  std::int32_t Node::getParentIndex()
  {
    return parentIndex;
  }

  std::tuple<float, std::int32_t> Node::getRecord(std::int32_t index)
  {
    if (index >= MAX_RECORDS || index < 0)
      throw std::runtime_error("Index out of bounds");
    return records[index];
  }

  std::int32_t Node::getId()
  {
    return this->id;
  }

  std::array<char, NODE_SIZE> Node::toBytes()
  {
    std::array<char, NODE_SIZE> bytes;
    std::size_t offset = 0;
    std::memcpy(bytes.data() + offset, &size, sizeof(size));
    offset += sizeof(size);
    std::memcpy(bytes.data() + offset, &parentIndex, sizeof(parentIndex));
    offset += sizeof(parentIndex);
    std::memcpy(bytes.data() + offset, &isLeaf, sizeof(isLeaf));
    offset += sizeof(isLeaf);
    for (std::int32_t i = 0; i < MAX_RECORDS; ++i)
    {
      auto &record = records[i];
      std::memcpy(bytes.data() + offset, &std::get<0>(record), sizeof(std::get<0>(record)));
      offset += sizeof(std::get<0>(record));
      std::memcpy(bytes.data() + offset, &std::get<1>(record), sizeof(std::get<1>(record)));
      offset += sizeof(std::get<1>(record));
    }
    for (std::int32_t i = 0; i < MAX_POINTERS; ++i)
    {
      std::memcpy(bytes.data() + offset, &pointers[i], sizeof(pointers[i]));
      offset += sizeof(pointers[i]);
    }
    return bytes;
  }

  void Node::fromBytes(std::array<char, NODE_SIZE> bytes)
  {
    std::size_t offset = 0;
    std::memcpy(&size, bytes.data() + offset, sizeof(size));
    offset += sizeof(size);
    std::memcpy(&parentIndex, bytes.data() + offset, sizeof(parentIndex));
    offset += sizeof(parentIndex);
    std::memcpy(&isLeaf, bytes.data() + offset, sizeof(isLeaf));
    offset += sizeof(isLeaf);
    for (std::int32_t i = 0; i < MAX_RECORDS; ++i)
    {
      float key;
      std::int32_t address;
      std::memcpy(&key, bytes.data() + offset, sizeof(key));
      offset += sizeof(key);
      std::memcpy(&address, bytes.data() + offset, sizeof(address));
      offset += sizeof(address);
      std::tuple<float, std::int32_t> record = std::make_tuple(key, address);
      records[i] = record;
    }
    for (std::int32_t i = 0; i < MAX_POINTERS; ++i)
    {
      std::memcpy(&pointers[i], bytes.data() + offset, sizeof(pointers[i]));
      offset += sizeof(pointers[i]);
    }
  }

  bool Node::getIsLeaf()
  {
    return isLeaf;
  }

  bool Node::isDirty()
  {
    return dirty;
  }

  Node::Node(const Node &other) : pointers(other.pointers), size(other.size), isLeaf(other.isLeaf), parentIndex(other.parentIndex), records(other.records), id(other.id)
  {
  }

  Node &Node::operator=(const Node &other)
  {
    if (this == &other)
      return *this;
    id = other.id;
    pointers = other.pointers;
    size = other.size;
    isLeaf = other.isLeaf;
    parentIndex = other.parentIndex;
    records = other.records;
    return *this;
  }

  Node &Node::operator=(Node &&other)
  {
    if (this != &other)
    {
      pointers = std::move(other.pointers);
      size = other.size;
      isLeaf = other.isLeaf;
      parentIndex = other.parentIndex;
      records = std::move(other.records);

      id = other.id;
      other.size = 0;
      other.isLeaf = true;
      other.parentIndex = -1;
      other.pointers.fill(-1);
      for (auto &record : other.records)
      {
        record = std::make_tuple(-1, -1);
      }
    }
    return *this;
  }
}