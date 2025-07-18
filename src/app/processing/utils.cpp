#include "utils.h"

#include <algorithm>
#include <utility>

size_t Processing::indexOfMaxRealElemFromComplexArray(ConstSpectrumSpan array)
{
    return std::max_element(std::begin(array), std::end(array), [](auto& a, auto& b){
        return a.real() < b.real();
           }) - std::begin(array);
}
