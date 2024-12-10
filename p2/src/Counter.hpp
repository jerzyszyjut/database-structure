#ifndef COUNTERS_HPP
#define COUNTERS_HPP

namespace sbd
{
  class Counters
  {
  public:
    static Counters &getInstance()
    {
      static Counters instance;
      return instance;
    }

    void incrementRead()
    {
      if (enabled)
        ++readCounter;
    }
    void incrementWrite()
    {
      if (enabled)
        ++writeCounter;
    }

    int getReadCounter() const { return readCounter; }
    int getWriteCounter() const { return writeCounter; }

    void disable() { enabled = false; }
    void enable() { enabled = true; }

    void reset()
    {
      readCounter = 0;
      writeCounter = 0;
      enabled = true;
    }

  private:
    int readCounter = 0;
    int writeCounter = 0;
    bool enabled = true;

    Counters() = default;
    ~Counters() = default;
    Counters(const Counters &) = delete;
    Counters &operator=(const Counters &) = delete;
  };
} // namespace sbd

#endif // COUNTERS_HPP