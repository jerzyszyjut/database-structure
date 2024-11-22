#ifndef COUNTERS_HPP
#define COUNTERS_HPP

namespace sbd
{
    class Counters
    {
    public:
        // Singleton access
        static Counters &getInstance()
        {
            static Counters instance;
            return instance;
        }

        // Increment methods
        void incrementRead() { ++readCounter; }
        void incrementWrite() { ++writeCounter; }
        void incrementPhases() { ++phasesCounter; }

        // Getter methods
        int getReadCounter() const { return readCounter; }
        int getWriteCounter() const { return writeCounter; }
        int getPhasesCounter() const { return phasesCounter; }

        // Reset method
        void reset()
        {
            readCounter = 0;
            writeCounter = 0;
            phasesCounter = 0;
        }

    private:
        int readCounter = 0;
        int writeCounter = 0;
        int phasesCounter = 0;

        // Private constructors to enforce singleton usage
        Counters() = default;
        ~Counters() = default;
        Counters(const Counters &) = delete;
        Counters &operator=(const Counters &) = delete;
    };
} // namespace sbd

#endif // COUNTERS_HPP
