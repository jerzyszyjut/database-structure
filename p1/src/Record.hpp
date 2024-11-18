#ifndef RECORD_HPP
#define RECORD_HPP

#include <array>
#include <vector>
#include <iostream>
#include <numeric>
#include <cstring>

namespace sbd
{
  static constexpr auto PAGE_SIZE = 200;
  static constexpr auto RECORD_SIZE = 20;

  template <typename T>
  class Record
  {
  private:
    std::array<T, 5> numbers;
    static double computeAverage(const std::array<T, 5> &nums)
    {
      return std::accumulate(nums.begin(), nums.end(), T(0)) / static_cast<double>(nums.size());
    }

  public:
    Record() : numbers({0, 0, 0, 0, 0})
    {
      std::size_t byteSize = sizeof(T) * numbers.size();
      if (byteSize != RECORD_SIZE)
      {
        throw std::runtime_error("Record size is not correct");
      }
    }
    Record(const std::array<T, 5> &inputNumbers) : numbers(inputNumbers) {}
    bool operator==(const Record<T> &other) const
    {
      return computeAverage(numbers) == computeAverage(other.numbers);
    }
    bool operator>(const Record<T> &other) const
    {
      return computeAverage(numbers) > computeAverage(other.numbers);
    }
    bool operator<(const Record<T> &other) const
    {
      return computeAverage(numbers) < computeAverage(other.numbers);
    }
    bool operator>=(const Record<T> &other) const
    {
      return !(*this < other);
    }
    bool operator<=(const Record<T> &other) const
    {
      return !(*this > other);
    }
    std::vector<char> toBytes() const
    {
      std::vector<char> bytes(RECORD_SIZE);
      std::memcpy(bytes.data(), numbers.data(), RECORD_SIZE);
      return bytes;
    }
    void fromBytes(const std::vector<char> &bytes)
    {
      std::memcpy(numbers.data(), bytes.data(), RECORD_SIZE);
    }
    friend std::ostream &operator<<(std::ostream &os, const Record<T> &record)
    {
      os << "[ ";
      for (const auto &num : record.numbers)
      {
        os << num << " ";
      }
      os << "]";
      return os;
    }
  };
}
#endif
