#include <string>
#include <random>
#include "Record.hpp"
#include "Tape.hpp"
#include "algorithms.hpp"

namespace sbd
{
  void createRandomTape(const std::string &filename, std::size_t numRecords)
  {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(1, 100);

    sbd::Tape<int> tape(filename, std::ios_base::out);
    for (std::size_t i = 0; i < numRecords; ++i)
    {
      std::array<int, 5> numbers;
      for (std::size_t j = 0; j < numbers.size(); ++j)
      {
        numbers[j] = dist(gen);
      }
      sbd::Record<int> record(numbers);
      tape.write(record);
    }
  }

  void distributeBetweenTapes(sbd::Tape<int> &inputTape, sbd::Tape<int> &outputTape1, sbd::Tape<int> &outputTape2)
  {
    inputTape.reset();
    outputTape1.reset();
    outputTape2.reset();

    bool firstTape = true;
    sbd::Record<int> previousRecord;
    if (!inputTape.eof())
    {
      previousRecord = inputTape.read();
      outputTape1.write(previousRecord);
    }
    else
    {
      throw std::runtime_error("Input tape is empty");
    }

    while (!inputTape.eof())
    {
      sbd::Record<int> record = inputTape.read();
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
    }
  }

  bool merge(sbd::Tape<int> &outputTape, sbd::Tape<int> &tape1, sbd::Tape<int> &tape2)
  {
    bool sorted = true;

    tape1.reset();
    tape2.reset();
    outputTape.reset();

    if (tape1.eof())
    {
      while (!tape2.eof())
      {
        outputTape.write(tape2.read());
      }
      return sorted;
    }
    if (tape2.eof())
    {
      while (!tape1.eof())
      {
        outputTape.write(tape1.read());
      }
      return sorted;
    }

    sbd::Record<int> record1 = tape1.read();
    sbd::Record<int> record2 = tape2.read();
    sbd::Record<int> previousRecord = record1 < record2 ? record1 : record2;

    while (!tape1.eof() && !tape2.eof())
    {
      if (record1 < record2)
      {
        outputTape.write(record1);
        if (previousRecord > record1)
        {
          sorted = false;
        }
        previousRecord = record1;
        record1 = tape1.read();
      }
      else
      {
        outputTape.write(record2);
        if (previousRecord > record2)
        {
          sorted = false;
        }
        previousRecord = record2;
        record2 = tape2.read();
      }
    }

    while (!tape1.eof())
    {
      outputTape.write(record1);
      if (previousRecord > record1)
      {
        sorted = false;
      }
      previousRecord = record1;
      record1 = tape1.read();
    }

    while (!tape2.eof())
    {
      outputTape.write(record2);
      if (previousRecord > record2)
      {
        sorted = false;
      }
      previousRecord = record2;
      record2 = tape2.read();
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
    while (!sorted)
    {
      sorted = merge(outputTape, tape1, tape2);
      distributeBetweenTapes(outputTape, tape1, tape2);
    }
  }
} // namespace sbd