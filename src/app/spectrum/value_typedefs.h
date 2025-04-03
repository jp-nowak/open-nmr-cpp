#ifndef VALUE_TYPEDEFS_H
#define VALUE_TYPEDEFS_H

#include <complex>
#include <vector>
#include <span>

typedef std::complex<double> FidComplexValue;
typedef FidComplexValue SpectrumComplexValue;
typedef std::vector<FidComplexValue> FidType;
typedef FidType SpectrumType;
typedef std::span<FidComplexValue> FidSpan;
typedef FidSpan SpectrumSpan;

#endif // VALUE_TYPEDEFS_H
