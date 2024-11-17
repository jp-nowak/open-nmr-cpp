#ifndef PGENERAL_H
#define PGENERAL_H

#include "../3rd_party/kissfft/kiss_fft.h"

#include <vector>
#include <complex>

namespace Processing {
    std::vector<kiss_fft_cpx> perform_fft(const std::vector<std::complex<double>>& fid);
    std::vector<std::complex<double>> fft_to_spectrum(const std::vector<kiss_fft_cpx>& fid);
    std::vector<std::complex<double>> generate_spectrum_from_fid(const std::vector<std::complex<double>>& fid);
}


#endif // GENERAL_H
