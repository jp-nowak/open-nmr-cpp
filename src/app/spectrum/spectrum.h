#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "spectrum_info.h"
#include "../file_io/general.h"

#include <vector>
#include <complex>
#include <string>
#include <memory>


class Spectrum
{
public:
    Spectrum(const SpectrumInfo& info, const std::vector<std::complex<double>>& fid, const std::vector<std::complex<double>>& spectrum);
    static Spectrum from_file_read_result(FileIO::FileReadResult result);
    static std::unique_ptr<Spectrum> pointer_from_file_read_result(FileIO::FileReadResult result);

    const std::vector<std::complex<double>>& get_spectrum();

    SpectrumInfo info;


private:
    std::vector<std::complex<double>> fid;
    std::vector<std::complex<double>> spectrum;
};


#endif // SPECTRUM_H
