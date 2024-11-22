#include <string>
#include <random>
#include "Record.hpp"
#include "Tape.hpp"
#include "algorithms.hpp"

namespace sbd
{
  int createRandomTape(const std::string &filename, std::size_t numRecords)
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    gen.seed(193064); // Set a fixed seed for reproducibility
    std::uniform_int_distribution<int> dist(1, 100);
    int numberOfSeries = 0;

    sbd::Tape<int> tape(filename, std::ios_base::out);
    sbd::Record<int> previousRecord;
    for (std::size_t i = 0; i < numRecords; ++i)
    {
      std::array<int, 5> numbers;
      for (std::size_t j = 0; j < numbers.size(); ++j)
      {
        numbers[j] = dist(gen);
      }
      sbd::Record<int> record(numbers);
      if (previousRecord > record)
      {
        numberOfSeries++;
      }
      previousRecord = record;
      tape.write(record);
    }

    return numberOfSeries;
  }

  void distributeBetweenTapes(sbd::Tape<int> &inputTape, sbd::Tape<int> &outputTape1, sbd::Tape<int> &outputTape2)
  {
    inputTape.changeMode(std::ios_base::in);
    outputTape1.changeMode(std::ios_base::out);
    outputTape2.changeMode(std::ios_base::out);

    bool firstTape = true;
    sbd::Record<int> previousRecord;

    if (!inputTape.eof())
    {
      previousRecord = inputTape.getNextRecord();
      outputTape1.write(previousRecord);
    }
    else
    {
      throw std::runtime_error("Input tape is empty");
    }

    while (!inputTape.eof())
    {
      sbd::Record<int> record = inputTape.getNextRecord();
      if (previousRecord > record)
      {
        firstTape = !firstTape;
      }

      if (firstTape)
      {
        outputTape1.write(record);
      }
      else
      {
        outputTape2.write(record);
      }
      previousRecord = record;
    }
  }

  bool merge(sbd::Tape<int> &outputTape, sbd::Tape<int> &tape1, sbd::Tape<int> &tape2)
  {
    bool sorted = true;

    tape1.changeMode(std::ios_base::in);
    tape2.changeMode(std::ios_base::in);
    outputTape.changeMode(std::ios_base::out);

    if (tape1.eof())
    {
      while (!tape2.eof())
      {
        outputTape.write(tape2.getNextRecord());
      }
      return sorted;
    }
    if (tape2.eof())
    {
      while (!tape1.eof())
      {
        outputTape.write(tape1.getNextRecord());
      }
      return sorted;
    }

    sbd::Record<int> previousRecord = tape1.getCurrentRecord() < tape2.getCurrentRecord() ? tape1.getCurrentRecord() : tape2.getCurrentRecord();
    sbd::Record<int> recordToWrite;

    while (!tape1.eof() && !tape2.eof())
    {
      if (tape1.getCurrentRecord() > tape2.getCurrentRecord())
      {
        recordToWrite = tape2.getNextRecord();
        outputTape.write(recordToWrite);
        if (previousRecord > recordToWrite) sorted = false;
        previousRecord = recordToWrite;
      }
      else
      {
        recordToWrite = tape1.getNextRecord();
        outputTape.write(recordToWrite);
        if (previousRecord > recordToWrite) sorted = false;
        previousRecord = recordToWrite;
      }
    }

    while (!tape1.eof())
    {
      recordToWrite = tape1.getNextRecord();
      outputTape.write(recordToWrite);
      if (previousRecord > recordToWrite) sorted = false;
      previousRecord = recordToWrite;
    }

    while (!tape2.eof())
    {
      recordToWrite = tape2.getNextRecord();
      outputTape.write(recordToWrite);
      if (previousRecord > recordToWrite) sorted = false;
      previousRecord = recordToWrite;
    }

    return sorted;
  }

  void sortTape(const std::string &inputFilename, const std::string &outputFilename)
  {
    sbd::Tape<int> inputTape(inputFilename, std::ios_base::in);
    sbd::Tape<int> tape1("tape1.dat", std::ios_base::out);
    sbd::Tape<int> tape2("tape2.dat", std::ios_base::out);

    distributeBetweenTapes(inputTape, tape1, tape2);

    sbd::Tape<int> outputTape(outputFilename, std::ios_base::out);
    bool sorted = false;
    std::cout << outputTape << std::endl;
    while (!sorted)
    {
      sorted = merge(outputTape, tape1, tape2);
      std::cout << outputTape << std::endl;
      distributeBetweenTapes(outputTape, tape1, tape2);
      Counters::getInstance().incrementPhases();
    }
  }
} // namespace sbd