#ifndef PHASE_CORRECTION_H
#define PHASE_CORRECTION_H

#include <vector>
#include <complex>

namespace Processing
{

struct Ph0
{
    double ph0;
};

struct Ph1
{
    double pivot;
    double ph1;
};

struct Phase
{
    Ph0 ph0;
    Ph1 ph1;
};

void operator*= (std::vector<std::complex<double>>& a, Ph0 b);
void operator*= (std::vector<std::complex<double>>& a, Ph1 b);
} // Processing








#endif // PHASE_CORRECTION_H
