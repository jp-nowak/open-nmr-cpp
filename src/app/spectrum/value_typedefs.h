#ifndef VALUE_TYPEDEFS_H
#define VALUE_TYPEDEFS_H

#include "../typedefs.h"

#include "spectrum_info.h"

#include <complex>
#include <vector>
#include <span>

typedef std::span<Complex> FidSpan;
typedef FidSpan SpectrumSpan;
typedef std::span<Complex const> ConstFidSpan;
typedef ConstFidSpan ConstSpectrumSpan;

using IntegralsVector = Vector<IntegralRecord>;

#endif // VALUE_TYPEDEFS_H
