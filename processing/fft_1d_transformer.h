#ifndef FFT_1D_TRANSFORMER_H
#define FFT_1D_TRANSFORMER_H

#include <vector>
#include <complex>

#include "../3rd_party/kissfft-master/kiss_fft.h"

namespace Processing
{

    class FFT_1D_Transformer
    {
    public:
        FFT_1D_Transformer(const std::vector<std::complex<double>>& fid);
        ~FFT_1D_Transformer();
        void transform();
        std::vector<std::complex<double>> receive();
        std::vector<kiss_fft_cpx> receive_raw();

    private:
        kiss_fft_cfg buffer;
        std::vector<kiss_fft_cpx> fft_in;
    };

}
#endif // FFT_1D_TRANSFORMER_H
