#ifndef TAPE_SORTER_HPP
#define TAPE_SORTER_HPP

#include <string>
#include <stdexcept>
#include <random>
#include <array>
#include "Record.hpp"
#include "Tape.hpp"

namespace sbd
{

    void createRandomTape(const std::string &filename, std::size_t numRecords);
    void distributeBetweenTapes(Tape<int> &inputTape, Tape<int> &outputTape1, Tape<int> &outputTape2);
    bool merge(Tape<int> &outputTape, Tape<int> &tape1, Tape<int> &tape2);
    void sortTape(const std::string &inputFilename, const std::string &outputFilename);

} // namespace sbd

#endif
