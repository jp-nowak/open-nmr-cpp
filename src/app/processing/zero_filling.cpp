#include "zero_filling.h"

#include <cassert>

void Processing::zeroFillToNextPowerOf2(std::vector<std::complex<double>>& fid)
{
    size_t n = 0;
    size_t l = fid.size();
    for (auto i : POWERS_OF_TWO) {
        if (i > l) {
            n = i;
            break;
        }
    }
    if (n == 0) return;
    fid.resize(n, std::complex<double>{0.0, 0.0});

    assert((fid.size() % 2) == 0 && fid.size() > l);
}

void Processing::zeroFillToNumber(std::vector<std::complex<double>>& fid, size_t n)
{
    if (n < fid.size()) return;

    fid.resize(n, std::complex<double>{0.0, 0.0});
}

