#ifndef TYPEDEFS_H
#define TYPEDEFS_H

#endif // TYPEDEFS_H

#include <cstddef>
#include <complex>
#include <vector>
#include <string_view>
#include <unordered_map>
#include <array>
#include <cstdint>

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

template <typename T>
using Vector = std::vector<T>;

template <typename T, size_t N>
using Array = std::array<T, N>;

template <size_t N>
using FixedString = std::array<char, N>;

using Byte = u8;

using NumericValueType = double;
using Complex = std::complex<NumericValueType>;
using ComplexVector = Vector<Complex>;
using String = std::string;
using Buffer = Vector<Byte>;
using StringView = std::string_view;

