#ifndef PGENERAL_H
#define PGENERAL_H

#include "../spectrum/value_typedefs.h"

#include "../3rd_party/kissfft/kiss_fft.h"

#include <vector>
#include <span>
#include <complex>

namespace Processing
{

    /*!
     * \brief perform_fft does fft on data, uses FFT_1D_Transformer
     * \param fid data to be transformed, size should be a power of two, they are not modified
     * \return fft transformed data, as std::vector<kiss_fft_cpx> (one more operation iterating over a vector
     *  is required to form spectrum, so it makes no sense to make std::vector<std::complex<double>> at this point)
     */
    std::vector<kiss_fft_cpx> perform_fft(std::span<FidComplexValue const> fid);

    /*!
     * \brief fft_to_spectrum creates a spectrum out of fft transformed data by conrotating them ie.
     * given initial vector: | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | as std::vector<kiss_fft_cpx>
     *              returns: | 3 | 2 | 1 | 0 | 7 | 6 | 5 | 4 | as std::vector<std::complex<double>>
     * \param fid: data after fft from which spectrum is to be prepared
     * \return
     */
    std::vector<SpectrumComplexValue> fft_to_spectrum(const std::vector<kiss_fft_cpx>& fftFid);

    /*!
     * \brief generate_spectrum_from_fid makes spectrum out of fid by using perform_fft and fft_to_spectrum
     * \param fid
     * \return
     */
    std::vector<SpectrumComplexValue> generate_spectrum_from_fid(std::span<FidComplexValue const> fid);

}


#endif // GENERAL_H
