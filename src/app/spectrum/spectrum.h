#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "spectrum_info.h"
#include "../file_io/general.h"

#include <vector>
#include <complex>
#include <string>



class Spectrum
{
public:
    Spectrum(const SpectrumInfo& info, const std::vector<std::complex<double>>& fid, const std::vector<std::complex<double>>& spectrum);

    const SpectrumInfo info;

private:
    std::vector<std::complex<double>> fid;
    std::vector<std::complex<double>> spectrum;
};

Spectrum create_spectrum_from_file_read_result(FileIO::FileReadResult result);

#endif // SPECTRUM_H
