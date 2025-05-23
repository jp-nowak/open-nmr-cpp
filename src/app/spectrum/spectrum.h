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
    const FidSizeInfo& getFidSizeInfo() const;


    void setPh0(const Processing::Ph0& phase);
    void setPh1(const Processing::Ph1& phase);

    void zeroFill(size_t n);
    void truncate(size_t n);

    void integrate(size_t start, size_t end) const;
    void recalcIntegrals(size_t previousSpectrumSize) const;

    SpectrumInfo info;

    mutable IntegralsVector integrals;



private:

    void generateSpectrum();

    void restorePhase();


    std::vector<std::complex<double>> fid;

    std::vector<std::complex<double>> spectrum;

    Processing::Phase phaseCorrection;

    FidSizeInfo fidSizeInfo;

};

// recalculates .relativeValue in IntegralRecord's in integrals according to .relativeValue = .absoluteValue / valueOfOne
void recalcRelativeIntegralsValues(IntegralsVector& integrals, double valueOfOne);

// calls .clear() on integrals
void resetIntegrals(IntegralsVector& integrals);



#endif // SPECTRUM_H
