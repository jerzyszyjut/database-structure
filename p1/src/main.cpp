#include <iostream>
#include <array>
#include <fstream>
#include <random>

#include "Record.hpp"
#include "Tape.hpp"

void populateTape(sbd::Tape<int> &tape)
{
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dis(1, 1000);

  for (int i = 0; i < 1000; i++)
  {
    std::array<int, 5> numbers;
    for (int j = 0; j < 5; j++)
    {
      numbers[j] = dis(gen);
    }
    sbd::Record<int> record(numbers);
    tape.addRecord(record);
  }
}

int main()
{
  sbd::Tape<int> mainTape("data.dat", std::ios_base::in | std::ios_base::binary);
  sbd::Tape<int> tape1("data1.dat", std::ios_base::out | std::ios_base::binary);
  sbd::Tape<int> tape2("data2.dat", std::ios_base::out | std::ios_base::binary);

  sbd::Record<int> previousRecord = mainTape.getNextRecord();
  tape1.addRecord(previousRecord);
  bool currentTape = true;

  while(!mainTape.isEnd())
  {
    sbd::Record<int> currentRecord = mainTape.getNextRecord();
    if (currentRecord < previousRecord)
    {
      currentTape = !currentTape;
    }

    if (currentTape)
    {
      tape1.addRecord(currentRecord);
    }
    else
    {
      tape2.addRecord(currentRecord);
    }
    
    previousRecord = currentRecord;
  }

  tape1.closeFile();
  tape2.closeFile();
  mainTape.closeFile();

  return 0;
}
