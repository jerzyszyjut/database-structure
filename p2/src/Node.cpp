#include "Node.hpp"

namespace sbd
{
  Node::Node() : keys(), addresses(), pointers(), size(0), isLeaf(true)
  {
    keys.fill(-1);
    addresses.fill(-1);
    pointers.fill(-1);
  }

  Node::~Node()
  {
  }

  // Node &Node::operator=(const Node &other)
  // {
  //   keys = other.keys;
  //   addresses = other.addresses;
  //   pointers = other.pointers;
  //   size = other.size;
  //   isLeaf = other.isLeaf;
  //   return *this;
  // }

  // Node::Node(const Node &other) : keys(other.keys), addresses(other.addresses), pointers(other.pointers), size(other.size), isLeaf(other.isLeaf)
  // {
  // }

  // Node &Node::operator=(Node &&other)
  // {
  //   keys = std::move(other.keys);
  //   addresses = std::move(other.addresses);
  //   pointers = std::move(other.pointers);
  //   size = other.size;
  //   isLeaf = other.isLeaf;
  //   return *this;
  // }

  // Node::Node(Node &&other) : keys(std::move(other.keys)), addresses(std::move(other.addresses)), pointers(std::move(other.pointers)), size(other.size), isLeaf(other.isLeaf)
  // {
  // }
}