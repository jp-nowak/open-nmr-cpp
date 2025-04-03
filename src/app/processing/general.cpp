
#include "general.h"
#include "fft_1d_transformer.h"

#include "algorithm"

#include <cassert>
#include <numeric>

std::vector<kiss_fft_cpx> Processing::perform_fft(std::span<const FidComplexValue> fid, FidSizeInfo info)
{
    FFT1DTransformer t{fid, info};
    t.transform();
    return t.receiveRaw();
}

std::vector<SpectrumComplexValue> Processing::fft_to_spectrum(const std::vector<kiss_fft_cpx>& fftFid)
{
    std::vector<std::complex<double>> spectrum{};
    spectrum.resize(fftFid.size());

    for (size_t i = 0, j = fftFid.size() / 2 - 1, k = fftFid.size() / 2, l = fftFid.size() - 1;
        i < fftFid.size() / 2;
        i++, j--, k++, l--) {

        spectrum.at(i) = std::complex<double>{fftFid.at(j).r, fftFid.at(j).i};
        spectrum.at(k) = std::complex<double>{fftFid.at(l).r, fftFid.at(l).i};
    }
    return spectrum;
}

std::vector<SpectrumComplexValue> Processing::generate_spectrum_from_fid(std::span<FidComplexValue const> fid, FidSizeInfo info)
{
    return fft_to_spectrum(perform_fft(fid, info));
}

std::vector<kiss_fft_cpx> Processing::constructFid(std::span<FidComplexValue const> fid, FidSizeInfo info)
{
    std::vector<kiss_fft_cpx> processedFid(info.zeroFilledTo, kiss_fft_cpx{0.0, 0.0});

    for (size_t i = 0; i < info.truncationStart; i++) {
        processedFid[i].r = fid[i].real();
        processedFid[i].i = fid[i].imag();
    }

    if (!(info.groupDelay == 0.0)) {
        std::rotate(processedFid.begin(), processedFid.begin() + static_cast<size_t>(info.groupDelay), processedFid.end());
    }

    return processedFid;
}

SpectrumSpan::value_type::value_type Processing::integrateByTrapezoidRule(SpectrumSpan values)
{
    assert(values.size() > 2);

    using ComplexValue = decltype(values)::value_type;
    using SubValue = decltype(values)::value_type::value_type;

    using std::real;

    SubValue deltaX = values.size() / values.size();

    SubValue firstAndLast = (real(*values.begin()) + real(*(values.end() - 1))) / 2;



    SubValue sum = std::accumulate(values.begin() + 1, values.end() - 1, SubValue{},
    [](ComplexValue a, ComplexValue b) -> SubValue {
    return real(a) + real(b);
    });

    return deltaX * (firstAndLast + sum);
}
