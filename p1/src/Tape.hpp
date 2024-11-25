#pragma once

#include <vector>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <iostream>

#include "Record.hpp"
#include "Counters.hpp"
#include "constants.hpp"

namespace sbd
{
  template <typename T>
  class Tape
  {
  private:
    std::vector<Record<T>> records;
    std::string filename;
    std::fstream file;
    std::size_t currentRecord;
    std::ios_base::openmode mode;
    bool endOfFile;
    static constexpr std::size_t BLOCK_SIZE = TAPE_SIZE * RECORD_SIZE;

  public:
    Tape(const std::string &fname, std::ios_base::openmode mode)
        : filename(fname), currentRecord(0), mode(mode), endOfFile(false)
    {
      mode |= std::ios_base::binary;
      file.open(filename, mode);
      if (!file.is_open())
      {
        throw std::runtime_error("Could not open file " + filename);
      }

      if (mode & std::ios_base::in)
      {
        readBlock();
      }
    }

    ~Tape()
    {
      if (file.is_open())
      {
        if (mode & std::ios_base::out)
        {
          writeBlock();
        }
        file.close();
      }
    }

    void resetRead()
    {
      records.clear();
      currentRecord = 0;
    }

  private:
    void checkReadMode()
    {
      if (!(mode & std::ios_base::in))
      {
        throw std::runtime_error("Tape is not in read mode");
      }
    }

    void checkWriteMode()
    {
      if (!(mode & std::ios_base::out))
      {
        throw std::runtime_error("Tape is not in write mode");
      }
    }

    void readBlock()
    {
      checkReadMode();

      records.clear();
      currentRecord = 0;

      char buffer[BLOCK_SIZE] = {0};
      file.read(buffer, BLOCK_SIZE);
      Counters::getInstance().incrementRead();

      std::size_t bytesRead = file.gcount();
      if (bytesRead == 0)
      {
        endOfFile = true;
        return;
      }

      std::size_t numRecords = bytesRead / RECORD_SIZE;
      for (std::size_t i = 0; i < numRecords; ++i)
      {
        Record<T> record;
        std::memcpy(&record, buffer + (i * RECORD_SIZE), RECORD_SIZE);
        records.push_back(record);
      }
    }

    void writeBlock()
    {
      checkWriteMode();

      if (records.empty())
        return;

      char buffer[BLOCK_SIZE] = {0};
      std::size_t bufferIndex = 0;

      for (const auto &record : records)
      {
        if (bufferIndex + RECORD_SIZE > BLOCK_SIZE)
        {
          break;
        }
        std::memcpy(buffer + bufferIndex, &record, RECORD_SIZE);
        bufferIndex += RECORD_SIZE;
      }

      file.write(buffer, bufferIndex);
      file.flush();
      Counters::getInstance().incrementWrite();

      records.clear();
      currentRecord = 0;
    }

  public:
    void changeMode(std::ios_base::openmode newMode)
    {
      if (file.is_open())
      {
        if (mode & std::ios_base::out)
          writeBlock();
        file.close();
      }

      mode = newMode | std::ios_base::binary;
      file.open(filename, mode);

      if (!file.is_open())
      {
        throw std::runtime_error("Could not open file " + filename + " in new mode");
      }

      if (newMode & std::ios_base::in)
      {
        readBlock();
      }
    }

    Record<T> getCurrentRecord()
    {
      checkReadMode();

      if (currentRecord >= records.size())
      {
        readBlock();
      }

      return records[currentRecord];
    }

    Record<T> getNextRecord()
    {
      checkReadMode();

      if (currentRecord >= records.size())
      {
        readBlock();
      }

      return records[currentRecord++];
    }

    void write(const Record<T> &record)
    {
      checkWriteMode();
      records.push_back(record);
      if (records.size() >= TAPE_SIZE)
      {
        writeBlock();
      }
    }

    bool eof() const
    {
      return currentRecord >= records.size() && records.empty() && endOfFile;
    }

    friend std::ostream &operator<<(std::ostream &os, Tape<T> &tape)
    {
      Counters::getInstance().disable();
      tape.resetRead();
      tape.changeMode(std::ios_base::in);
      os << "Tape: " << tape.filename << std::endl;
      os << "Mode: " << (tape.mode & std::ios_base::in ? "read" : "write") << std::endl;
      os << "Current record: " << tape.currentRecord << std::endl;
      os << "End of file: " << (tape.endOfFile ? "yes" : "no") << std::endl;
      os << "Records: " << std::endl;
      while (!tape.eof())
      {
        os << tape.getNextRecord() << std::endl;
      }
      Counters::getInstance().enable();
      return os;
    }
  };
} // namespace sbd
