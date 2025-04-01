#include "zero_filling.h"

#include <cassert>

// void Processing::zeroFillToNextPowerOf2(std::vector<std::complex<double>>& fid)
// {
//     size_t l = fid.size();
//     size_t n = nextPowerOf2(l);

//     // size_t n = 0; // OLD VERSION
//     // for (auto i : POWERS_OF_TWO) {
//     //     if (i > l) {
//     //         n = i;
//     //         break;
//     //     }
//     // }

//     if (n == 0) return;
//     fid.resize(n, std::complex<double>{0.0, 0.0});

//     assert((fid.size() % 2) == 0 && fid.size() > l);
// }

// void Processing::zeroFillToNumber(std::vector<std::complex<double>>& fid, size_t n)
// {
//     if (n < fid.size()) return;

//     fid.resize(n, std::complex<double>{0.0, 0.0});
// }

size_t Processing::nextPowerOf2(size_t number)
{
    size_t n = 0;
    for (auto i : POWERS_OF_TWO) {
        if (i > number) {
            n = i;
            break;
        }
    }
    return n;
}

size_t Processing::closestPowerOf2(size_t number)
{
    size_t n = 0;
    for (auto i : POWERS_OF_TWO) {
        if (i >= number) {
            n = i;
            break;
        }
    }
    return n;
}
