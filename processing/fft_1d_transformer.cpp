#include "fft_1d_transformer.h"

#include "../3rd_party/kissfft-master/kiss_fft.h"

Processing::FFT_1D_Transformer::FFT_1D_Transformer(const std::vector<std::complex<double>>& fid)
: fft_in{}
{
    buffer = kiss_fft_alloc(fid.size(), 0, NULL, NULL);

    fft_in.resize(fid.size());

    for (size_t i = 0; i < fid.size(); i++) {
        fft_in[i].r = fid[i].real();
        fft_in[i].i = fid[i].imag();
    }
}

Processing::FFT_1D_Transformer::~FFT_1D_Transformer()
{
    kiss_fft_free(buffer);
}

void Processing::FFT_1D_Transformer::transform()
{
    kiss_fft(buffer, fft_in.data(), fft_in.data());
}

std::vector<std::complex<double>> Processing::FFT_1D_Transformer::receive()
{
    std::vector<std::complex<double>> result{};
    result.resize(fft_in.size());

    for (size_t i = 0; i < fft_in.size(); i++) {
        result[i] = std::complex<double>{fft_in[i].r, fft_in[i].i};
    }
    return result;
}

std::vector<kiss_fft_cpx> Processing::FFT_1D_Transformer::receive_raw()
{
    return fft_in;
}
