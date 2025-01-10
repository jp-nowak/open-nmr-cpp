#ifndef SPECTRUM_VIEW_H
#define SPECTRUM_VIEW_H

#include <vector>
#include <complex>

class SpectrumView
{
public:
    SpectrumView(std::vector<std::complex<double>>* spectrum);
    void setRange(size_t begin, size_t end);
    const std::complex<double>& operator[](size_t n);
    std::vector<std::complex<double>>::size_type size();
    void reset();
    size_t maxElemIndex;
    const size_t trueSize;
private:
    void initialize();
    std::vector<std::complex<double>>* const spectrum;
    size_t begin;
    size_t end;

};

#endif // SPECTRUM_VIEW_H
