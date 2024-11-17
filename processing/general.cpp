
#include "general.h"
#include "fft_1d_transformer.h"

std::vector<kiss_fft_cpx> Processing::perform_fft(const std::vector<std::complex<double>>& fid)
{
    FFT_1D_Transformer t{fid};
    t.transform();
    return t.receive_raw();
}

std::vector<std::complex<double>> Processing::fft_to_spectrum(const std::vector<kiss_fft_cpx>& fid)
{
    std::vector<std::complex<double>> spectrum{};
    spectrum.resize(fid.size());

    for (size_t i = 0, j = fid.size() / 2 - 1, k = fid.size() / 2, l = fid.size() - 1;
        i < fid.size() / 2;
        i++, j--, k++, l--) {

        spectrum.at(i) = std::complex<double>{fid.at(j).r, fid.at(j).i};
        spectrum.at(k) = std::complex<double>{fid.at(l).r, fid.at(l).i};
    }
    return spectrum;
}

std::vector<std::complex<double>> Processing::generate_spectrum_from_fid(const std::vector<std::complex<double>>& fid)
{
    return fft_to_spectrum(perform_fft(fid));
}

