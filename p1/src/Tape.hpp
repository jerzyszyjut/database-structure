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
        readFile();
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
      if (mode == this->mode)
      {
        return;
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
        readFile();
      }
    }

    void checkReadMode()
    {
      if (!file.is_open() || file.rdstate() || file.eof())
      {
        changeMode(std::ios_base::in);
      }
    }

    void checkWriteMode()
    {
      if (!file.is_open() || file.rdstate())
      {
        changeMode(std::ios_base::out);
      }
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

    // void writeFile()
    // {
    //   checkWriteMode();
    //   for (const auto &record : records)
    //   {
    //     std::vector<char> bytes = record.toBytes();
    //     file.write(bytes.data(), bytes.size());
    //   }
    // }

  public:
    void write(const Record<T> &record)
    {
      checkWriteMode();
      records.push_back(record);
      std::vector<char> bytes = record.toBytes();
      file.write(bytes.data(), bytes.size());
    }

    Record<T> read()
    {
      checkReadMode();
      if (currentRecord >= records.size())
      {
        throw std::runtime_error("No more records to read");
      }
      return records[currentRecord++];
    }

    void reset()
    {
      checkReadMode();
      file.clear();
      file.seekg(0, std::ios_base::beg);
    }

    bool eof() const
    {
      return currentRecord >= records.size();
    }
  };
} // namespace sbd
