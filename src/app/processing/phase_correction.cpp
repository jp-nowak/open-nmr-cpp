#include "phase_correction.h"


#include <cassert>
#include <numbers>
#include <algorithm>

void Processing::operator*= (spectrumType& lhs, Ph0 rhs)
{
    using namespace std::complex_literals;
    if (rhs.ph0 == 0.0) return;
    assert(std::isnormal(rhs.ph0));
    std::complex<double> firstHalf = rhs.ph0 * std::numbers::pi * 1.0i;
    firstHalf = std::exp(firstHalf);

    for (auto& i : lhs) {
        i *= firstHalf;
    }
}

void Processing::operator*= (spectrumType& lhs, Ph1 rhs)
{
    if (rhs.ph1 == 0.0) return;
    assert(lhs.size() % 2 == 0 && "size should be divisible by 2");
    assert(std::isnormal(rhs.ph1));
    assert((rhs.pivot >= 0.0) and (rhs.pivot <= 100));

    using namespace std::complex_literals;
    double pivot = -(rhs.pivot / 100 - 0.5) * 2;

    std::vector<double> firstHalf{};
    size_t halfSize = lhs.size() / 2;
    firstHalf.resize(halfSize);

    {
        size_t i = 0;
        std::generate(firstHalf.begin(), firstHalf.end(), [&](){
            return static_cast<double>(i++) / halfSize;
        });
    }

    std::vector<double> multipliers{firstHalf.rbegin(), firstHalf.rend()};
    std::for_each(multipliers.begin(), multipliers.end(), [&](double& d) { d *= -1;});

    multipliers.insert(multipliers.end(), firstHalf.begin(), firstHalf.end());
    std::for_each(multipliers.begin(), multipliers.end(), [&](double& d) { d += pivot;});

    std::vector<std::complex<double>> correction;
    correction.resize(lhs.size());
    {
        size_t i = 0;
        std::generate(correction.begin(), correction.end(), [&](){
            std::complex<double> z = rhs.ph1 * multipliers[i++] * std::numbers::pi * 1.0i;
            return std::exp(z);
        });
    }

    assert(lhs.size() == correction.size());

    for (size_t i = 0; i < lhs.size(); i++) {
        lhs[i] = lhs[i] * correction[i];
    }

}
