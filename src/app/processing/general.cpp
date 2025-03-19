
#include "general.h"
#include "fft_1d_transformer.h"

std::vector<kiss_fft_cpx> Processing::perform_fft(std::span<const FidComplexValue> fid)
{
    FFT_1D_Transformer t{fid};
    t.transform();
    return t.receive_raw();
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

std::vector<SpectrumComplexValue> Processing::generate_spectrum_from_fid(std::span<FidComplexValue const> fid)
{
    return fft_to_spectrum(perform_fft(fid));
}

