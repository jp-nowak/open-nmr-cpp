#ifndef PHASE_CORRECTION_H
#define PHASE_CORRECTION_H

#include <vector>
#include <complex>

namespace Processing
{

// phase correction are stated using radians

struct Ph0
{
    double ph0;
};

struct Ph1
{
    double ph1;
    double pivot; // value between 0 (left edge of spectrum) and 100 (right edge of specturm)
};

struct Phase
{
    Ph0 ph0;
    Ph1 ph1;
};

// applies in place 0th order phase correction
void operator*= (std::vector<std::complex<double>>& a, Ph0 b);

// applies in place 1st order phase correction
void operator*= (std::vector<std::complex<double>>& a, Ph1 b);
} // Processing








#endif // PHASE_CORRECTION_H
