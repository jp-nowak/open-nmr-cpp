#ifndef VALUE_TYPEDEFS_H
#define VALUE_TYPEDEFS_H

#include "spectrum_info.h"

#include <complex>
#include <vector>
#include <span>

typedef std::complex<double> FidComplexValue;
typedef FidComplexValue SpectrumComplexValue;
typedef std::vector<FidComplexValue> FidType;
typedef FidType SpectrumType;
typedef std::span<FidComplexValue> FidSpan;
typedef FidSpan SpectrumSpan;
typedef std::span<FidComplexValue const> ConstFidSpan;
typedef ConstFidSpan ConstSpectrumSpan;

typedef std::vector<FidComplexValue> fidType;
typedef std::vector<SpectrumComplexValue> spectrumType;


typedef std::vector<IntegralRecord> IntegralsVector;

#endif // VALUE_TYPEDEFS_H
