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
  std::string inputFilename = "input.dat";
  std::string outputFilename = "output.dat";
  std::size_t numRecords = sbd::RECORD_COUNT;

  int numberOfSeries = sbd::createRandomTape(inputFilename, numRecords);

  //sbd::Tape<int> inputTape(inputFilename, std::ios_base::in);

  // std::cout << "Input tape:" << std::endl;
  // while (!inputTape.eof())
  // {
  //   sbd::Record<int> record = inputTape.getNextRecord();
  //   std::cout << record << std::endl;
  // }

  sbd::sortTape(inputFilename, outputFilename);

  // sbd::Tape<int> outputTape(outputFilename, std::ios_base::in);

  // std::cout << "Sorted tape:" << std::endl;
  // while (!outputTape.eof())
  // {
  //   sbd::Record<int> record = outputTape.getNextRecord();
  //   std::cout << record << std::endl;
  // }

  std::cout << "Number of series: " << numberOfSeries << std::endl;
  std::cout << "Expected number of phases: " << sbd::calculatePhaseCount(numberOfSeries, sbd::TAPE_SIZE) << std::endl;
  std::cout << "Actual number of phases: " << sbd::Counters::getInstance().getPhasesCounter() << std::endl;

  std::cout << "Expected number of read/write operations: " << sbd::calculateOperationsCount(numRecords, numberOfSeries, sbd::TAPE_SIZE) << std::endl;
  std::cout << "Actual number of read operations: " << sbd::Counters::getInstance().getReadCounter() << std::endl;
  std::cout << "Actual number of write operations: " << sbd::Counters::getInstance().getWriteCounter() << std::endl;
  std::cout << "Actual number of read/write operations: " << sbd::Counters::getInstance().getReadCounter() + sbd::Counters::getInstance().getWriteCounter() << std::endl;


  return 0;
}
