#include "fft_1d_transformer.h"

#include "../processing/general.h"

#include "../3rd_party/kissfft/kiss_fft.h"

Processing::FFT1DTransformer::FFT1DTransformer(std::span<Complex const> fid, FidSizeInfo info)
: fftIn{constructFid(fid, info)}
{
    buffer = kiss_fft_alloc(fftIn.size(), 0, NULL, NULL);
}

Processing::FFT1DTransformer::~FFT1DTransformer()
{
    kiss_fft_free(buffer);
}

void Processing::FFT1DTransformer::transform()
{
    kiss_fft(buffer, fftIn.data(), fftIn.data());
}

std::vector<Complex> Processing::FFT1DTransformer::receive()
{
    std::vector<std::complex<double>> result{};
    result.resize(fftIn.size());

    for (size_t i = 0; i < fftIn.size(); i++) {
        result[i] = std::complex<double>{fftIn[i].r, fftIn[i].i};
    }
    return result;
}

std::vector<kiss_fft_cpx> Processing::FFT1DTransformer::receiveRaw()
{
    return fftIn;
}
