#include "spectrum_view.h"

#include <algorithm>
#include <QDebug>

SpectrumView::SpectrumView(std::vector<std::complex<double>>* spectrum)
    : spectrum{spectrum}
    , begin{0}
    , trueSize{spectrum->size()}
    , end{spectrum->size()}

{
    initialize();
    trueRealMaximum = (*spectrum)[maxElemIndex].real();
}

void SpectrumView::initialize()
{
    maxElemIndex = std::max_element(spectrum->begin() + begin, spectrum->end(),
    [](const std::complex<double>& a, const std::complex<double>& b){return a.real() < b.real();})
    - spectrum->begin() - begin;
}

void SpectrumView::setRange(size_t begin, size_t end)
{
    if ((begin < size()) and (end <= size()) and (begin < end)) {
        this->end = this->begin + end;
        this->begin += begin;
        initialize();
    }
}

void SpectrumView::reset()
{
    begin = 0;
    end = trueSize;
    initialize();
}

std::vector<std::complex<double>>::size_type SpectrumView::size()
{
    return end - begin;
}

const std::complex<double>& SpectrumView::operator[](size_t n)
{
    return (*spectrum)[n + begin];
}
