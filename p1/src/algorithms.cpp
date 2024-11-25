#include <string>
#include <random>
#include "Record.hpp"
#include "Tape.hpp"
#include "algorithms.hpp"

namespace sbd
{
  int createRandomTape(const std::string &filename, std::size_t numRecords)
  {
    Counters::getInstance().disable();
    std::random_device rd;
    std::mt19937 gen(rd());
    gen.seed(1); // Set a fixed seed for reproducibility
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

    Counters::getInstance().enable();
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

    sbd::Record<int> previousRecord;
    sbd::Record<int> record1 = tape1.getCurrentRecord(), record2 = tape2.getCurrentRecord();
    sbd::Record<int> recordToWrite;
    while (!tape1.eof() && !tape2.eof())
    {
      bool hasSeriesOnTape1Ended = tape1.getCurrentRecord() < record1, hasSeriesOnTape2Ended = tape2.getCurrentRecord() < record2;

      if (hasSeriesOnTape1Ended && !tape2.eof())
      {
        bool flag = true;
        sbd::Record<int> record = tape2.getCurrentRecord();
        while (!tape2.eof() && tape2.getCurrentRecord() >= record)
        {
          flag = flag && tape2.getCurrentRecord() >= record;
          record = tape2.getCurrentRecord();
          recordToWrite = tape2.getNextRecord();
          outputTape.write(recordToWrite);
        }
        sorted = sorted && flag;
      }

      if (hasSeriesOnTape2Ended && !tape1.eof())
      {
        bool flag = true;
        sbd::Record<int> record = tape1.getCurrentRecord();
        while (!tape1.eof() && tape1.getCurrentRecord() >= record)
        {
          flag = flag && tape1.getCurrentRecord() >= record;
          record = tape1.getCurrentRecord();
          recordToWrite = tape1.getNextRecord();
          outputTape.write(recordToWrite);
        }
        sorted = sorted && flag;
      }

      record1 = tape1.getCurrentRecord();
      record2 = tape2.getCurrentRecord();
      recordToWrite = record1 < record2 ? tape1.getNextRecord() : tape2.getNextRecord();
      if (previousRecord > recordToWrite)
        sorted = false;
      outputTape.write(recordToWrite);
      previousRecord = recordToWrite;
    }

    if (!tape1.eof() && tape1.getCurrentRecord() < previousRecord)
      sorted = false;
    if (!tape2.eof() && tape2.getCurrentRecord() < previousRecord)
      sorted = false;

    while (!tape1.eof())
    {
      recordToWrite = tape1.getCurrentRecord();
      outputTape.write(recordToWrite);
      if (previousRecord > tape1.getNextRecord())
        sorted = false;
      previousRecord = recordToWrite;
    }

    while (!tape2.eof())
    {
      recordToWrite = tape2.getCurrentRecord();
      outputTape.write(recordToWrite);
      if (previousRecord > tape2.getNextRecord())
        sorted = false;
      previousRecord = recordToWrite;
    }

    return sorted;
  }

  void sortTape(const std::string &inputFilename, const std::string &outputFilename)
  {
    sbd::Tape<int> inputTape(inputFilename, std::ios_base::in);
    sbd::Tape<int> tape1("tape1.dat", std::ios_base::out);
    sbd::Tape<int> tape2("tape2.dat", std::ios_base::out);
    // std::cout << inputTape << std::endl;
    distributeBetweenTapes(inputTape, tape1, tape2);
    // std::cout << tape1 << std::endl;
    // std::cout << tape2 << std::endl;

    sbd::Tape<int> outputTape(outputFilename, std::ios_base::out);
    // std::cout << outputTape << std::endl;
    bool sorted = false;
    while (true)
    {
      // std::cout << "Phase " << Counters::getInstance().getPhasesCounter() << std::endl;
      sorted = merge(outputTape, tape1, tape2);
      // std::cout << outputTape << std::endl;
      if (sorted)
        break;
      distributeBetweenTapes(outputTape, tape1, tape2);
      // std::cout << tape1 << std::endl;
      // std::cout << tape2 << std::endl;

      Counters::getInstance().incrementPhases();
    }
    // std::cout << "Sorted tape:" << std::endl;
    // std::cout << outputTape << std::endl;
  }
} // namespace sbd