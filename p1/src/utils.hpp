#ifndef UTILS_HPP
#define UTILS_HPP

#include <cmath>

namespace sbd
{
    int calculateOperationsCount(int numberOfRecords, int numberOfSeries, int numberOfRecordsInBlock);
    int calculatePhaseCount(int numberOfSeries, int numberOfRecordsInBlock);
}

#endif // UTILS_HPP
