#include "phase_correction.h"

#include <cassert>
#include <numbers>
#include <algorithm>

void Processing::operator*= (std::vector<std::complex<double>>& lhs, Ph0 rhs)
{
    using namespace std::complex_literals;
    std::complex<double> mult = rhs.ph0 * std::numbers::pi * 1.0i;
    mult = std::exp(mult);

    for (auto& i : lhs) {
        i *= mult;
    }
}

void Processing::operator*= (std::vector<std::complex<double>>& lhs, Ph1 rhs)
{
    assert(lhs.size() % 2 == 0 && "size should be divisible by 2");

    using namespace std::complex_literals;
    double pivot = -(rhs.pivot - 0.5) * 2;

    std::vector<double> mult{};
    size_t halfSize = lhs.size() / 2;
    mult.resize(halfSize);

    {
        size_t i = 0;
        std::generate(mult.begin(), mult.end(), [&](){
            return i++ / halfSize;
        });
    }

    std::vector<double> otherHalf{mult.rbegin(), mult.rend()};
    mult.insert(mult.end(), otherHalf.begin(), otherHalf.end());
    std::for_each(mult.begin(), mult.end(), [&](double& d) { d += pivot;});

    std::vector<std::complex<double>> correction;
    correction.resize(lhs.size());
    {
        size_t i = 0;
        std::generate(correction.begin(), correction.end(), [&](){
            std::complex<double> z = rhs.ph1 * i++ * std::numbers::pi * 1.0i;
            return std::exp(z);
        });
    }

    assert(lhs.size() == correction.size());

    for (size_t i = 0; i < lhs.size(); i++) {
        lhs[i] = lhs[i] * correction[i];
    }

}
