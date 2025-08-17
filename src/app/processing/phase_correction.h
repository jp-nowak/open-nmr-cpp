#ifndef PHASE_CORRECTION_H
#define PHASE_CORRECTION_H


#include "../spectrum/value_typedefs.h"


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

/*!
 * \brief applies 0th order phase correction,
 * that is multiplies all elements by exp(Ph0.ph0 * pi * i)
 * \param lhs
 * \param rhs correction to be applied in pi * rad
 */
void operator*= (spectrumType& lhs, Ph0 rhs);

// applies in place 1st order phase correction
void operator*= (spectrumType& lhs, Ph1 rhs);
} // Processing








#endif // PHASE_CORRECTION_H
