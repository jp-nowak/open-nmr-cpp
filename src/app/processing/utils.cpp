#include "utils.h"

#include <algorithm>
#include <cmath>
#include <cassert>

size_t Processing::indexOfRealMaximum(ConstSpectrumSpan array)
{
    return std::max_element(std::begin(array), std::end(array), [](auto& a, auto& b){
               return std::real(a) < std::real(b);
           }) - std::begin(array);
}

size_t Processing::indexOfRealMinimum(ConstSpectrumSpan array)
{
    return std::min_element(std::begin(array), std::end(array), [](auto& a, auto& b){
               return std::real(a) < std::real(b);
           }) - std::begin(array);
}

double Processing::findRealMaximum(ConstSpectrumSpan array)
{
    return std::real(array[indexOfRealMaximum(array)]);
}

double Processing::findRealMinimum(ConstSpectrumSpan array)
{
    return std::real(array[indexOfRealMinimum(array)]);
}

void Processing::operator*= (ComplexVector& lhs, double rhs)
{
    assert(std::isnormal(rhs));
    for (auto& i : lhs) {
        i *= rhs;
    }
}

