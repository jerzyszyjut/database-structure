#include <iostream>
#include <array>
#include <fstream>
#include <random>

#include "Record.hpp"
#include "Tape.hpp"
#include "algorithms.hpp"
#include "utils.hpp"

int main()
{
  sbd::sortTape();

  std::cout << "Number of series: " << sbd::Counters::getInstance().getSeriesCounter() << std::endl;
  std::cout << "Expected number of phases: " << sbd::calculatePhaseCount(sbd::Counters::getInstance().getSeriesCounter(), sbd::TAPE_SIZE) << std::endl;
  std::cout << "Actual number of phases: " << sbd::Counters::getInstance().getPhasesCounter() << std::endl;

  std::cout << "Expected number of read/write operations: " << sbd::calculateOperationsCount(sbd::Counters::getInstance().getNumberOfRecords(), sbd::Counters::getInstance().getSeriesCounter(), sbd::TAPE_SIZE) << std::endl;
  std::cout << "Actual number of read operations: " << sbd::Counters::getInstance().getReadCounter() << std::endl;
  std::cout << "Actual number of write operations: " << sbd::Counters::getInstance().getWriteCounter() << std::endl;
  std::cout << "Actual number of read/write operations: " << sbd::Counters::getInstance().getReadCounter() + sbd::Counters::getInstance().getWriteCounter() << std::endl;

  return 0;
}
