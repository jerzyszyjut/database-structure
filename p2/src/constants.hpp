#pragma once
#include <cstddef>
#include <cstdint>

namespace sbd
{
  static constexpr auto TREE_FILE_NAME = "tree.dat";
  static constexpr auto DATA_FILE_NAME = "data.dat";
  static constexpr auto D_SIZE = 4;
  static constexpr auto VALUE_SIZE = sizeof(std::int32_t);
  static constexpr auto KEY_SIZE = sizeof(std::int32_t);
  static constexpr auto POINTER_SIZE = sizeof(std::int32_t);
  static constexpr auto RECORD_SIZE = 5 * VALUE_SIZE;
  static constexpr auto MAX_NODE_SIZE = 2 * D_SIZE * (KEY_SIZE + POINTER_SIZE) + POINTER_SIZE;
  static constexpr auto MIN_NODE_SIZE = D_SIZE * (KEY_SIZE + POINTER_SIZE) + POINTER_SIZE;
  static constexpr auto MAX_RECORDS = 2 * D_SIZE - 1;
  static constexpr auto MIN_RECORDS = D_SIZE - 1;
  static constexpr auto MIN_POINTERS = D_SIZE;
  static constexpr auto MAX_POINTERS = 2 * D_SIZE;
} // namespace sbd