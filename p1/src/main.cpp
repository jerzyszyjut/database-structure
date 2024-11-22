#include <iostream>
#include <array>
#include <fstream>
#include <random>

#include "Record.hpp"
#include "Tape.hpp"
#include "algorithms.hpp"

int main()
{
  std::string inputFilename = "input.dat";
  std::string outputFilename = "output.dat";
  std::size_t numRecords = 20;

  sbd::createRandomTape(inputFilename, numRecords);

  sbd::Tape<int> inputTape(inputFilename, std::ios_base::in);

  std::cout << "Input tape:" << std::endl;
  while (!inputTape.eof())
  {
    sbd::Record<int> record = inputTape.getNextRecord();
    std::cout << record << std::endl;
  }

  sbd::sortTape(inputFilename, outputFilename);

  sbd::Tape<int> outputTape(outputFilename, std::ios_base::in);

  std::cout << "Sorted tape:" << std::endl;
  while (!outputTape.eof())
  {
    sbd::Record<int> record = outputTape.getNextRecord();
    std::cout << record << std::endl;
  }

  return 0;
}
