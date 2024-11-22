#include <cmath>

namespace sbd
{
    int calculateOperationsCount(int numberOfRecords, int numberOfSeries, int numberOfRecordsInBlock)
    {
        return (int)(4 * numberOfRecords * std::ceil(std::log2(numberOfSeries))) / numberOfRecordsInBlock;
    }

    int calculatePhaseCount(int numberOfSeries, int numberOfRecordsInBlock)
    {
        return (int)std::ceil(std::log2(numberOfSeries));
    }

} // namespace sbd
