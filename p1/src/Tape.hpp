#include "Record.hpp"
#include <vector>
#include <fstream>
#include <stdexcept>
#include <cstring>

namespace sbd
{
  template <typename T>
  class Tape
  {
  private:
    std::vector<Record<T>> currentPage = {};
    std::ios_base::openmode mode;
    std::string filename;
    std::fstream file;
    std::size_t fileSize;
    std::size_t currentPageRecordIndex;
    std::size_t currentPageNumber;

  public:
    Tape(const std::string &filename, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out | std::ios_base::binary) : filename(filename), mode(mode), currentPageRecordIndex(0), currentPageNumber(0)
    {
      openFile();
      if (mode & std::ios_base::in)
      {
        file.seekg(0, std::ios::end);
        fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        readRecords();
      }
      else {
        fileSize = 0;
      }
    }

    ~Tape()
    {
      closeFile();
    }

    void addRecord(const Record<T> &record)
    {
      currentPage.push_back(record);
      currentPageRecordIndex = currentPage.size() - 1;
      writeRecord(record);
    }

    Record<T> getCurrentRecord()
    {
      return currentPage[currentPageRecordIndex];
    }

    Record<T> getNextRecord()
    {
      auto record = getCurrentRecord();

      if (currentPageNumber * PAGE_SIZE + currentPageRecordIndex * RECORD_SIZE >= fileSize)
      {
        throw std::runtime_error("No more records");
      }

      currentPageRecordIndex++;

      return record;
    }

    void closeFile()
    {
      file.close();
    }

    bool isEnd()
    {
      if (mode & std::ios_base::out)
      {
        return currentPageNumber * (PAGE_SIZE / RECORD_SIZE) + currentPageRecordIndex >= currentPage.size();
      }
    
      return isEndOfFile();
    }

    bool isEndOfFile()
    {
      return currentPageNumber * PAGE_SIZE + currentPageRecordIndex * RECORD_SIZE >= fileSize;
    }

    void reset()
    {
      currentPageRecordIndex = 0;
      currentPageNumber = 0;
    }

  private:
    void openFile()
    {
      file.open(filename, mode);
      if (!file.is_open())
      {
        throw std::runtime_error("Cannot open file");
      }
    }

    void writeRecord(const Record<T> &record)
    {
      std::vector<char> buffer = record.toBytes();
      file.write(buffer.data(), RECORD_SIZE);
      fileSize += RECORD_SIZE;
    } 

    void readRecords()
    {
      if(!file.good())
      {
        throw std::runtime_error("Cannot read records");
      }
      currentPage.clear();
      while(!file.eof())
      {
        Record<T> record;
        std::vector<char> buffer(RECORD_SIZE);
        file.read(buffer.data(), RECORD_SIZE);
        record.fromBytes(buffer);
        currentPage.push_back(record);
      } 
    }
  };
}
