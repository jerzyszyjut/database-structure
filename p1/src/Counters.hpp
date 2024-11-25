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
        void incrementPhases()
        {
            if (enabled)
                ++phasesCounter;
        }

        int getReadCounter() const { return readCounter; }
        int getWriteCounter() const { return writeCounter; }
        int getPhasesCounter() const { return phasesCounter; }

        void disable() { enabled = false; }
        void enable() { enabled = true; }

        void reset()
        {
            readCounter = 0;
            writeCounter = 0;
            phasesCounter = 0;
            enabled = true;
        }

    private:
        int readCounter = 0;
        int writeCounter = 0;
        int phasesCounter = 0;
        bool enabled = true;

        Counters() = default;
        ~Counters() = default;
        Counters(const Counters &) = delete;
        Counters &operator=(const Counters &) = delete;
    };
} // namespace sbd

#endif // COUNTERS_HPP
