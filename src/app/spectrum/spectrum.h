#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "spectrum_info.h"
#include "../file_io/general.h"
#include "../processing/phase_correction.h"

#include <vector>
#include <complex>
#include <memory>


class Spectrum
{

public:

    Spectrum(const SpectrumInfo& info, const std::vector<std::complex<double>>& fid, const std::vector<std::complex<double>>& spectrum);
    static Spectrum from_file_read_result(FileIO::FileReadResult result);
    static std::unique_ptr<Spectrum> pointer_from_file_read_result(FileIO::FileReadResult result);

    const std::vector<std::complex<double>>& get_spectrum();
    const Processing::Phase& getPhase();

    void setPh0(const Processing::Ph0& phase);
    void setPh1(const Processing::Ph1& phase);

    SpectrumInfo info;
    std::vector<std::complex<double>> spectrum;

private:
    std::vector<std::complex<double>> fid;

    Processing::Phase phaseCorrection;

};


#endif // SPECTRUM_H
