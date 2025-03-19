#ifndef ZERO_FILLING_H
#define ZERO_FILLING_H

#include <vector>
#include <complex>
#include <array>

namespace Processing
{

static const auto POWERS_OF_TWO = std::to_array<size_t>({
    // 1,
    // 2,
    // 4,
    // 8,
    // 16,
    // 32,
    // 64,
    // 128,
    // 256,
    // 512,
    1024,
    2048,
    4096,
    8192,
    16384,
    32768,
    65536,
    131072,
    262144,
    524288,
    1048576,
    2097152,
    4194304,
    8388608,
    16777216,
    // 33554432,
    // 67108864,
    // 134217728,
    // 268435456,
    // 536870912,
    // 1073741824,
    // 2147483648,
    // 4294967296,
    // 8589934592,
});

// appends zeros (0 + 0i) to fid until length reaches next power of two
// if length if already equal to power of two it is still appended to next one
void zeroFillToNextPowerOf2(std::vector<std::complex<double>>& fid);

// appends zeros (0 + 0i) to fid until length reaches given number
void zeroFillToNumber(std::vector<std::complex<double>>& fid, size_t n);

size_t nextPowerOf2(size_t number);

size_t closestPowerOf2(size_t number);

}


#endif // ZERO_FILLING_H
