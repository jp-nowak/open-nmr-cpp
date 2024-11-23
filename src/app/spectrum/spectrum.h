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
    static Spectrum from_file_read_result(FileIO::FileReadResult result);

    SpectrumInfo info;

private:
    std::vector<std::complex<double>> fid;
    std::vector<std::complex<double>> spectrum;
};


#endif // SPECTRUM_H
