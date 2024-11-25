#ifndef DEBUGCONTROLLER_HPP
#define DEBUGCONTROLLER_HPP

#include <iostream>
#include <random>
#include <optional>

#include "Tape.hpp"
#include "Record.hpp"
#include "Counters.hpp"

namespace sbd
{
  class DebugController
  {
  public:
    static DebugController &getInstance()
    {
      static DebugController instance;
      return instance;
    }

    void setInputTape(Tape<int> *tape) { inputTape = tape; }
    void setTape1(Tape<int> *tape) { tape1 = tape; }
    void setTape2(Tape<int> *tape) { tape2 = tape; }
    void setOutputTape(Tape<int> *tape) { outputTape = tape; }

    void enable() { enabled = true; }
    void disable() { enabled = false; }

    void printTape()
    {
      if (!enabled)
        return;

      std::cout << "Choose tape to print:" << std::endl;
      std::cout << "1 - input tape" << std::endl;
      std::cout << "2 - tape 1" << std::endl;
      std::cout << "3 - tape 2" << std::endl;
      std::cout << "4 - output tape" << std::endl;
      std::cout << "q - quit" << std::endl;
      std::cout << "Choice: ";

      char choice;
      std::cin >> choice;
      bool correct = false;
      while (!correct)
      {
        switch (choice)
        {
        case '1':
        case '2':
        case '3':
        case '4':
          correct = true;
          break;
        case 'q':
          return;
        default:
          std::cout << "Invalid choice" << std::endl;
          std::cin >> choice;
          break;
        }
      }
      switch (choice)
      {
      case '1':
        std::cout << *inputTape << std::endl;
        break;
      case '2':
        std::cout << *tape1 << std::endl;
        break;
      case '3':
        std::cout << *tape2 << std::endl;
        break;
      case '4':
        std::cout << *outputTape << std::endl;
        break;
      default:
        break;
      }
    }

    void addRandomRecordsToTapeRandomly()
    {
      if (!enabled)
        return;

      std::cout << "Enter number of records to add: ";
      std::size_t numRecords;
      std::cin >> numRecords;

      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<int> dis(1, 10000);

      inputTape->changeMode(std::ios_base::out);

      std::optional<Record<int>> previousRecord = std::nullopt;

      for (std::size_t i = 0; i < numRecords; ++i)
      {
        std::array<int, 5> numbers;
        for (std::size_t j = 0; j < numbers.size(); ++j)
        {
          numbers[j] = dis(gen);
        }
        Record<int> record(numbers);
        if (previousRecord.has_value() && previousRecord.value() > record)
        {
          Counters::getInstance().incrementSeries();
        }
        previousRecord = record;

        inputTape->write(record);
      }

      sbd::Counters::getInstance().setNumberOfRecords(numRecords);

      std::cout << "Added " << numRecords << " records to input tape" << std::endl;
    }

    void addRecordsToTapeManually()
    {
      if (!enabled)
        return;

      std::cout << "Enter number of records to add: ";
      std::size_t numRecords;
      std::cin >> numRecords;

      inputTape->changeMode(std::ios_base::out);

      std::optional<Record<int>> previousRecord = std::nullopt;

      for (std::size_t i = 0; i < numRecords; ++i)
      {
        std::array<int, 5> numbers;
        std::cout << "Enter 5 numbers separated by space: ";
        for (std::size_t j = 0; j < numbers.size(); ++j)
        {
          std::cin >> numbers[j];
        }
        Record<int> record(numbers);
        if (previousRecord.has_value() && previousRecord.value() > record)
        {
          Counters::getInstance().incrementSeries();
        }
        previousRecord = record;

        inputTape->write(record);
      }

      sbd::Counters::getInstance().setNumberOfRecords(numRecords);

      std::cout << "Added " << numRecords << " records to input tape" << std::endl;
    }

    void createBinaryFileWithNumbers()
    {
      if (!enabled)
        return;

      std::cout << "Enter number of records to add: ";
      std::size_t numRecords;
      std::cin >> numRecords;

      std::cout << "Enter filename: ";
      std::string filename;
      std::cin >> filename;

      std::random_device rd;
      std::mt19937 gen(rd());
      std::uniform_int_distribution<int> dis(1, 10000);

      std::ofstream file(filename, std::ios_base::out | std::ios_base::binary);

      std::optional<Record<int>> previousRecord = std::nullopt;

      std::optional<Record<int>> recordToWrite = std::nullopt;

      for (std::size_t i = 0; i < numRecords; ++i)
      {
        std::array<int, 5> numbers;
        for (std::size_t j = 0; j < numbers.size(); ++j)
        {
          numbers[j] = dis(gen);
        }
        Record<int> record(numbers);
        if (previousRecord.has_value() && previousRecord.value() > record)
        {
          Counters::getInstance().incrementSeries();
        }
        previousRecord = record;

        file.write(record.toBytes().data(), RECORD_SIZE);
      }
      Counters::getInstance().setNumberOfRecords(numRecords);

      file.close();

      std::cout << "Created binary file " << filename << " with " << numRecords << " records" << std::endl;
    }

    void changeTapeToLoadFromSpecificFile()
    {
      if (!enabled)
        return;

      std::cout << "Enter filename: ";
      std::string filename;
      std::cin >> filename;

      std::cout << "Choose tape to change:" << std::endl;
      std::cout << "1 - input tape" << std::endl;
      std::cout << "2 - tape 1" << std::endl;
      std::cout << "3 - tape 2" << std::endl;
      std::cout << "4 - output tape" << std::endl;
      std::cout << "q - quit" << std::endl;
      std::cout << "Choice: ";

      char choice;
      std::cin >> choice;
      bool correct = false;
      while (!correct)
      {
        switch (choice)
        {
        case '1':
        case '2':
        case '3':
        case '4':
          correct = true;
          break;
        case 'q':
          return;
        default:
          std::cout << "Invalid choice" << std::endl;
          std::cin >> choice;
          break;
        }
      }

      std::ifstream file(filename);
      if (!file.is_open())
      {
        std::cout << "Could not open file " << filename << std::endl;
        return;
      }
      std::size_t fileSize = std::filesystem::file_size(filename);

      switch (choice)
      {
      case '1':
        Counters::getInstance().setNumberOfRecords(fileSize / RECORD_SIZE);
        inputTape->changeFilename(filename);
        break;
      case '2':
        tape1->changeFilename(filename);
        break;
      case '3':
        tape2->changeFilename(filename);
        break;
      case '4':
        outputTape->changeFilename(filename);
        break;
      default:
        break;
      }
    }

    void printMessage()
    {
      std::cout << "Stopped by debug controller. Available commands:" << std::endl;
      std::cout << "n - continue" << std::endl;
      std::cout << "p - print tape" << std::endl;
      std::cout << "a - add random records to tape" << std::endl;
      std::cout << "i - add records to tape manually" << std::endl;
      std::cout << "b - create binary file with numbers" << std::endl;
      std::cout << "c - change tape to load from specific file" << std::endl;
      std::cout << "s - stop before sorting (toggle). Currently: " << (stopBeforeSorting ? "on" : "off") << std::endl;
      std::cout << "m - stop after merge (toggle). Currently: " << (stopAfterMerge ? "on" : "off") << std::endl;
      std::cout << "d - stop after distribute (toggle). Currently: " << (stopAfterDistribute ? "on" : "off") << std::endl;
      std::cout << "o - stop after sort (toggle). Currently: " << (stopAfterSort ? "on" : "off") << std::endl;
      std::cout << "q - quit" << std::endl;
    }

    void waitForInput(char breakpoint)
    {
      if (!enabled)
        return;

      if (breakpoint == 's' && !stopBeforeSorting)
        return;
      if (breakpoint == 's')
        std::cout << "Stopped before sorting" << std::endl;
      if (breakpoint == 'm' && !stopAfterMerge)
        return;
      if (breakpoint == 'm')
        std::cout << "Stopped after merge" << std::endl;
      if (breakpoint == 'd' && !stopAfterDistribute)
        return;
      if (breakpoint == 'd')
        std::cout << "Stopped after distribute" << std::endl;
      if (breakpoint == 'o' && !stopAfterSort)
        return;
      if (breakpoint == 'o')
        std::cout << "Stopped after sort" << std::endl;

      sbd::Counters::getInstance().disable();
      std::string input;
      printMessage();
      std::cin >> input;
      while (input != "q")
      {
        if (input == "n")
          break;
        else if (input == "p")
          printTape();
        else if (input == "a")
          addRandomRecordsToTapeRandomly();
        else if (input == "i")
          addRecordsToTapeManually();
        else if (input == "b")
          createBinaryFileWithNumbers();
        else if (input == "c")
          changeTapeToLoadFromSpecificFile();
        else if (input == "s")
          stopBeforeSorting = !stopBeforeSorting;
        else if (input == "m")
          stopAfterMerge = !stopAfterMerge;
        else if (input == "d")
          stopAfterDistribute = !stopAfterDistribute;
        else if (input == "o")
          stopAfterSort = !stopAfterSort;
        else
          std::cout << "Invalid command" << std::endl;

        printMessage();

        std::cin >> input;
      }

      sbd::Counters::getInstance().enable();
    }

  private:
    bool enabled = true;
    bool stopBeforeSorting = true;
    bool stopAfterMerge = false;
    bool stopAfterDistribute = false;
    bool stopAfterSort = false;
    Tape<int> *inputTape, *tape1, *tape2, *outputTape;

    DebugController() = default;
    ~DebugController() = default;
    DebugController(const DebugController &) = delete;
    DebugController &operator=(const DebugController &) = delete;
  };
} // namespace sbd

#endif // DEBUGCONTROLLER_HPP
