#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "spectrum_info.h"
#include "value_typedefs.h"
#include "../file_io/general.h"
#include "../processing/phase_correction.h"

#include <vector>
#include <complex>
#include <memory>
#include <span>

class Spectrum final
{

public:

    Spectrum(const SpectrumInfo& info, const std::vector<std::complex<double>>& fid);
    static std::unique_ptr<Spectrum> pointer_from_file_read_result(FileIO::FileReadResult result);

    std::span<SpectrumComplexValue const> get_spectrum() const;
    const Processing::Phase& getPhase() const;

    void setPh0(const Processing::Ph0& phase);
    void setPh1(const Processing::Ph1& phase);

    // append 0 + 0i to fid until it has size n, n should be power of 2
    void zeroFillOrTruncate(size_t n);

    SpectrumInfo info;

private:

    void generateSpectrum();

    std::vector<std::complex<double>> fid;

    std::vector<std::complex<double>> spectrum;


    Processing::Phase phaseCorrection;

    const size_t initialSize;

    #ifdef DEBUG__
    const size_t trueInitialSize;
    #endif
};


#endif // SPECTRUM_H
