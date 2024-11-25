#pragma once

#include <vector>
#include <fstream>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <filesystem>

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
    std::size_t currentRecordIndex;
    std::ios_base::openmode mode;
    std::size_t fileSize;
    std::size_t bytesProcessed;
    static constexpr std::size_t BLOCK_SIZE = TAPE_SIZE * RECORD_SIZE;

  public:
    Tape(const std::string &fname, std::ios_base::openmode mode)
        : filename(fname), currentRecordIndex(0), mode(mode), bytesProcessed(0)
    {
      mode |= std::ios_base::binary;
      openFile();
    }

    ~Tape()
    {
      closeFile();
    }

    void resetRead()
    {
      records.clear();
      currentRecordIndex = 0;
      bytesProcessed = 0;
      openFile();
    }

    void changeFilename(const std::string &newFilename)
    {
      closeFile();
      filename = newFilename;
      openFile();
    }

    std::size_t getFileSize() const
    {
      return fileSize;
    }

  private:
    void openFile()
    {
      closeFile();

      file.open(filename, mode);
      if (!file.is_open())
      {
        throw std::runtime_error("Could not open file " + filename);
      }

      bytesProcessed = 0;
      fileSize = std::filesystem::file_size(filename);

      if (mode & std::ios_base::in)
      {
        loadPage();
      }
    }

    void closeFile()
    {
      if (file.is_open())
      {
        if (mode & std::ios_base::out && !records.empty())
        {
          savePage();
        }
        records.clear();
        currentRecordIndex = 0;
        file.close();
      }
    }

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

    void loadPage()
    {
      checkReadMode();
      records.clear();
      currentRecordIndex = 0;

      char buffer[BLOCK_SIZE] = {0};
      file.read(buffer, BLOCK_SIZE);
      Counters::getInstance().incrementRead();

      std::size_t bytesRead = file.gcount();
      bytesProcessed += bytesRead;

      std::size_t numRecords = bytesRead / RECORD_SIZE;
      for (std::size_t i = 0; i < numRecords; ++i)
      {
        Record<T> record;
        std::memcpy(&record, buffer + (i * RECORD_SIZE), RECORD_SIZE);
        records.push_back(record);
      }
    }

    void savePage()
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
      Counters::getInstance().incrementWrite();
      file.flush();
      records.clear();
      currentRecordIndex = 0;
    }

  public:
    void changeMode(std::ios_base::openmode newMode)
    {
      closeFile();
      mode = newMode | std::ios_base::binary;
      openFile();
    }

    Record<T> getCurrentRecord()
    {
      checkReadMode();

      if (currentRecordIndex >= records.size())
      {
        if (bytesProcessed <= fileSize)
        {
          loadPage();
        }
        else
        {
          throw std::runtime_error("End of file reached");
        }
      }

      return records[currentRecordIndex];
    }

    Record<T> getNextRecord()
    {
      Record<T> record = getCurrentRecord();
      ++currentRecordIndex;
      return record;
    }

    void write(const Record<T> &record)
    {
      checkWriteMode();
      records.push_back(record);
      if (records.size() * RECORD_SIZE >= BLOCK_SIZE)
      {
        savePage();
      }
    }

    bool eof() const
    {
      return bytesProcessed >= fileSize && currentRecordIndex >= records.size();
    }

    friend std::ostream &operator<<(std::ostream &os, Tape<T> &tape)
    {
      Counters::getInstance().disable();
      tape.changeMode(std::ios_base::in);

      os << "Tape: " << tape.filename << std::endl;
      os << "Mode: " << (tape.mode & std::ios_base::in ? "read" : "write") << std::endl;
      os << "Current record index: " << tape.currentRecordIndex << std::endl;
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
