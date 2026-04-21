#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "spectrum_info.h"
#include "value_typedefs.h"
#include "../file_io/general.h"
#include "../processing/phase_correction.h"
#include "../processing/peak_finding.h"

#include <vector>
#include <complex>
#include <memory>
#include <span>
#include <set>

class Spectrum_1D final
{

public:

    Spectrum_1D(const SpectrumInfo& info, const std::vector<std::complex<double>>& fid);
    static std::unique_ptr<Spectrum_1D> uPtrFromReadResult(ReadResult result);


    std::span<Complex const> get_spectrum() const;
    const Processing::Phase& getPhase() const;
    const FidSizeInfo& getFidSizeInfo() const;


    void setPh0(const Processing::Ph0& phase);
    void setPh1(const Processing::Ph1& phase);

    void zeroFill(size_t n);
    void truncate(size_t n);

    void integrate(size_t start, size_t end) const;
    void recalcIntegrals(size_t previousSpectrumSize) const;

    void refreshDependentMembers(size_t previousSpectrumSize) const;

    void autoFindPeaks() const;

// public members
//---------------------------------------------------------------------------------------------------------------------
    SpectrumInfo info;

    //! integrals store integrals of measured on spectrum, it is modified by other classes and functions
    //! without oversight from spectrum
    //! position [0] stores absolute value corresponding to relative value of one
    mutable std::vector<IntegralRecord> integrals;
    mutable double integralRelativeOneValue = 0.0;
    mutable std::vector<PeakFinding::Peak> autoPeakList;
    // those members have no impact on state of class so they are mutable

private:

    // new spectrum is generated from fid
    void generateSpectrum();

    void restorePhase();


// private members
//---------------------------------------------------------------------------------------------------------------------


    ComplexVector fid;

    ComplexVector spectrum;

    // applied phase correction
    Processing::Phase phaseCorrection;

    // data used in preparation of fid for fft:
    // initial size, applied zero filling and truncation, group delay of fid
    FidSizeInfo fidSizeInfo;

};

// free functions
//---------------------------------------------------------------------------------------------------------------------


//! recalculates .relativeValue in IntegralRecord's in integrals according to .relativeValue = .absoluteValue / valueOfOne
void recalcRelativeIntegralsValues(const Spectrum_1D* spectrum, double valueOfOne);

//! calls .clear() on integrals
void resetIntegrals(std::vector<IntegralRecord>& integrals);

//! deletes deletedIntegral from integrals record
void deleteIntegral(std::vector<IntegralRecord>& integrals, IntegralRecord* deletedIntegral);


#endif // SPECTRUM_H
