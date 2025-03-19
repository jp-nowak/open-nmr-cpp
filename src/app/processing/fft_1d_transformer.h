#ifndef FFT_1D_TRANSFORMER_H
#define FFT_1D_TRANSFORMER_H

#include "../spectrum/value_typedefs.h"

#include "../3rd_party/kissfft/kiss_fft.h"

#include <vector>
#include <complex>
#include <span>

namespace Processing
{

    /*!
     * \brief The FFT_1D_Transformer class used for managing fft transformation done by kissfft library, in particular its memory buffer
     */
    class FFT_1D_Transformer
    {
    public:
        /*!
         * \brief FFT_1D_Transformer: creates object which will manage fft and buffer needed for its working
         * \param fid: data to be fourier transformed
         */
        FFT_1D_Transformer(std::span<FidComplexValue const> fid);

        // frees buffer
        ~FFT_1D_Transformer();

        // does fft transform over fft_in in place
        void transform();

        // returns fft transformed data as std::vector<std::complex<double>> - which is normal representation for this program
        std::vector<SpectrumComplexValue> receive();

        // returns fft transformed data as std::vector<kiss_fft_cpx> - which is a representation used by kissfft
        std::vector<kiss_fft_cpx> receive_raw();

    private:
        // buffer managed by this class, necessary for kissfft
        kiss_fft_cfg buffer;

        // data to be transformed, they are transformed in place
        std::vector<kiss_fft_cpx> fft_in;
    };

}
#endif // FFT_1D_TRANSFORMER_H
