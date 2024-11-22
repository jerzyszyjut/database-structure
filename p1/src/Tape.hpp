#pragma once

#include <vector>
#include <fstream>
#include <stdexcept>
#include <cstring>

#include "Record.hpp"
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

  public:
    Tape(const std::string &fname, std::ios_base::openmode mode) : filename(fname), currentRecord(0), mode(mode)
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
        file.close();
      }
    }

  private:
    void changeMode(std::ios_base::openmode mode)
    {
      if (this->mode == mode)
      {
        return;
      }
      if (records.size() > 0 && (this->mode & std::ios_base::out))
      {
        writeBlock();
      }
      if (file.is_open())
      {
        file.close();
      }
      file.open(filename, mode);
      if (!file.is_open())
      {
        throw std::runtime_error("Could not open file " + filename);
      }
      this->mode = mode;
      if (mode & std::ios_base::in)
      {
        currentRecord = 0;
        records.clear();
        readBlock();
      }
    }

    void checkReadMode()
    {
      changeMode(std::ios_base::in);
    }

    void checkWriteMode()
    {
      changeMode(std::ios_base::out);
    }

    void readFile()
    {
      checkReadMode();
      records.clear();
      currentRecord = 0;
      while (!file.eof())
      {
        Record<T> record;
        std::vector<char> bytes(RECORD_SIZE);
        file.read(bytes.data(), RECORD_SIZE);
        if (file.gcount() == 0)
        {
          break;
        }
        record.fromBytes(bytes);
        records.push_back(record);
      }
    }

    void readBlock()
    {
      checkReadMode();
      records.clear();
      currentRecord = 0;
      std::vector<char> bytes(RECORD_SIZE * TAPE_SIZE);
      file.read(bytes.data(), RECORD_SIZE * TAPE_SIZE);
      std::size_t recordsCount = file.gcount() / RECORD_SIZE;
      for (std::size_t i = 0; i < recordsCount; ++i)
      {
        Record<T> record;
        record.fromBytes(std::vector<char>(bytes.begin() + i * RECORD_SIZE, bytes.begin() + (i + 1) * RECORD_SIZE));
        records.push_back(record);
      }
    }

    void writeBlock()
    {
      checkWriteMode();
      std::vector<char> bytes;
      for (const auto &record : records)
      {
        std::vector<char> recordBytes = record.toBytes();
        bytes.insert(bytes.end(), recordBytes.begin(), recordBytes.end());
      }
      file.write(bytes.data(), bytes.size());
      records.clear();
    }

  public:
    Record<T> getCurrentRecord()
    {
      return records[currentRecord];
    }

    Record<T> getNextRecord()
    {
      checkReadMode();
      Record<T> record = getCurrentRecord();
      // std::cout << "Reading record: " << record << std::endl;
      incrementRecord();
      if (currentRecord >= records.size() && !file.eof())
      {
        readBlock();
        // std::cout << "Reading block" << std::endl;
      }
      return record;
    }

    void incrementRecord()
    {
      ++currentRecord;
    }

    void write(const Record<T> &record)
    {
      checkWriteMode();
      records.push_back(record);
      // std::cout << "Writing record: " << record << std::endl;
      if (records.size() >= TAPE_SIZE)
      {
        writeBlock();
        // std::cout << "Writing block" << std::endl;
      }
    }

    void reset(std::ios_base::openmode mode = std::ios_base::in)
    {
      if (mode & std::ios_base::in)
      {
        checkReadMode();
      }
      else
      {
        checkWriteMode();
      }
    }

    bool eof() const
    {
      return records.size() < TAPE_SIZE && currentRecord >= records.size();
    }
  };
} // namespace sbd
