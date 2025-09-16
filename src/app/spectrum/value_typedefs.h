#ifndef VALUE_TYPEDEFS_H
#define VALUE_TYPEDEFS_H

#include "spectrum_info.h"


#include <complex>
#include <vector>
#include <span>


template <typename T>
using Vector = std::vector<T>;

using Byte = std::byte;
using Value = double;
using Complex = std::complex<Value>;
using ComplexVector = Vector<Complex>;
using String = std::string;
using Buffer = Vector<Byte>;
using StringView = std::string_view;

typedef std::span<Complex> FidSpan;
typedef FidSpan SpectrumSpan;
typedef std::span<Complex const> ConstFidSpan;
typedef ConstFidSpan ConstSpectrumSpan;

using IntegralsVector = Vector<IntegralRecord>;

#endif // VALUE_TYPEDEFS_H
