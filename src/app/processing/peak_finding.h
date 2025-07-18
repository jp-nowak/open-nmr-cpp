#ifndef PEAK_FINDING_H
#define PEAK_FINDING_H

#include "../spectrum/value_typedefs.h"

#include <vector>

namespace PeakFinding
{

struct Point
{
    size_t x;
    double y;
};

struct Peak
{
    Point zenith;
    Point leftTrough;
    Point rightTrough;
    Point nidar;
};

double calcTreshold(ConstSpectrumSpan spectrum);



std::vector<Peak> simpleFindPeaks(ConstSpectrumSpan spectrum, double treshold);

}







#endif // PEAK_FINDING_H
