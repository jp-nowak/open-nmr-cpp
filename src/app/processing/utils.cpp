#include "utils.h"

#include <algorithm>
#include <utility>

size_t Processing::indexOfMaxRealElemFromComplexArray(ConstSpectrumSpan array)
{
    return std::max_element(std::begin(array), std::end(array), [](auto& a, auto& b){
               return std::real(a) < std::real(b);
           }) - std::begin(array);
}

double Processing::findRealMaximumFromVectorOfComplex(ConstSpectrumSpan array)
{
    return std::real(array[indexOfMaxRealElemFromComplexArray(array)]);
}
