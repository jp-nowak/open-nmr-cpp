#ifndef UTILS_H
#define UTILS_H

#include "../spectrum/value_typedefs.h"
#include "phase_correction.h"

namespace Processing
{

size_t indexOfRealMaximum(ConstSpectrumSpan array);

size_t indexOfRealMinimum(ConstSpectrumSpan array);

double findRealMaximum(ConstSpectrumSpan array);

double findRealMinimum(ConstSpectrumSpan array);

void operator*= (fidType& lhs, double rhs);

}


#endif // UTILS_H
