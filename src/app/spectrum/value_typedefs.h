#ifndef VALUE_TYPEDEFS_H
#define VALUE_TYPEDEFS_H

#include "../typedefs.h"


#include <span>

typedef std::span<Complex> FidSpan;
typedef FidSpan SpectrumSpan;
typedef std::span<Complex const> ConstFidSpan;
typedef ConstFidSpan ConstSpectrumSpan;


#endif // VALUE_TYPEDEFS_H
