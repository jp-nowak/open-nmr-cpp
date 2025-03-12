#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "spectrum_info.h"
#include "../file_io/general.h"
#include "../processing/phase_correction.h"

#include <vector>
#include <complex>
#include <memory>


class Spectrum final
{

public:

    Spectrum(const SpectrumInfo& info, const std::vector<std::complex<double>>& fid);
    static std::unique_ptr<Spectrum> pointer_from_file_read_result(FileIO::FileReadResult result);

    const std::vector<std::complex<double>>& get_spectrum() const;
    const Processing::Phase& getPhase() const;

    void setPh0(const Processing::Ph0& phase);
    void setPh1(const Processing::Ph1& phase);

    // append 0 + 0i to fid until it has size n, n should be power of 2
    void zeroFill(size_t n);

    SpectrumInfo info;
    std::vector<std::complex<double>> spectrum;

private:

    void generateSpectrum();

    std::vector<std::complex<double>> fid;

    Processing::Phase phaseCorrection;

};


#endif // SPECTRUM_H
