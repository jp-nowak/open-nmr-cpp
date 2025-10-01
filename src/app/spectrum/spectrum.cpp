#include "spectrum.h"
#include "../processing/general.h"
#include "../processing/utils.h"
#include "../processing/zero_filling.h"
#include <QDebug>

#include <algorithm>
#include <cassert>
#include <limits>

using namespace Processing;


Spectrum_1D::Spectrum_1D(const SpectrumInfo& info, const std::vector<std::complex<double>>& fid)
: info{info}
, fid{fid}
, phaseCorrection{Ph0{.ph0 = 0},
                  Ph1{.ph1 = 0, .pivot = 50}}
, fidSizeInfo{.initialSize = fid.size(),
              .truncationStart = fid.size(),
              .zeroFilledTo = nextPowerOf2(fid.size()),
              .groupDelay = info.group_delay}
{
    using namespace Processing;
    generateSpectrum();
    if (!(info.group_delay == 0.0)) {
        double decimalDelay = info.group_delay - static_cast<size_t>(info.group_delay);
        setPh1(Ph1{.ph1 = -decimalDelay, .pivot = 75});  // correcting phase distortion caused by group delay
    }
    if (findRealMaximum(get_spectrum()) < std::fabs(findRealMinimum(get_spectrum()))) {
        setPh0(Ph0{1}); // if highest point is negative flip spectrum
    }
    constexpr double MAXIMUM_Y_VALUE = 1000000;
    if (double maximum = findRealMaximum(get_spectrum()); maximum > MAXIMUM_Y_VALUE) {
        double mult = 1.0 / (maximum / MAXIMUM_Y_VALUE);
        this->fid *= mult; // downscaling fid if spectrum y values are too big
        generateSpectrum();
    }


#ifdef PEAK_FINDING_TEST
    autoFindPeaks();
#endif


}

void Spectrum_1D::generateSpectrum()
{
    spectrum = generate_spectrum_from_fid(std::span<Complex const>(this->fid), fidSizeInfo);
    restorePhase();
}

void Spectrum_1D::restorePhase()
{
    spectrum *= phaseCorrection.ph0;
    spectrum *= phaseCorrection.ph1;
}

std::unique_ptr<Spectrum_1D> Spectrum_1D::pointer_from_file_read_result(FileReadResult result)
{
    return std::make_unique<Spectrum_1D>(result.info, result.fids[0]);
}

std::span<Complex const> Spectrum_1D::get_spectrum() const
{
    return {spectrum};
}

const Phase& Spectrum_1D::getPhase() const
{
    return phaseCorrection;
}

const FidSizeInfo& Spectrum_1D::getFidSizeInfo() const
{
    return fidSizeInfo;
}

void Spectrum_1D::setPh0(const Ph0& phase)
{
    spectrum *= Ph0(phase.ph0 - phaseCorrection.ph0.ph0);
    phaseCorrection.ph0.ph0 = phase.ph0;
    refreshDependentMembers(spectrum.size());
}

void Spectrum_1D::setPh1(const Ph1& phase)
{
    if (phase.pivot == phaseCorrection.ph1.pivot) {
        spectrum *= Ph1(phase.ph1 - phaseCorrection.ph1.ph1, phase.pivot);
        phaseCorrection.ph1.ph1 = phase.ph1;
    } else {
        spectrum *= Ph1(-phaseCorrection.ph1.ph1, phaseCorrection.ph1.pivot);
        spectrum *= Ph1(phase.ph1, phase.pivot);
        phaseCorrection.ph1 = phase;
    }
    refreshDependentMembers(spectrum.size());
}

void Spectrum_1D::zeroFill(size_t n)
{
    assert(std::find(POWERS_OF_TWO.begin(), POWERS_OF_TWO.end(), n) != POWERS_OF_TWO.end());
    const size_t size = spectrum.size();

    fidSizeInfo.zeroFilledTo = n;

    if (n < fidSizeInfo.truncationStart) { // n lower then acquired fid size - truncation occurs
        fidSizeInfo.truncationStart = n;
    } /*else {
        if (fidSizeInfo.truncationStart < fidSizeInfo.initialSize) { // n higher then truncation start
            if (n > fidSizeInfo.initialSize) { // restoring initial size
                fidSizeInfo.truncationStart = fidSizeInfo.initialSize;
            } else { // restoring to n data points
                fidSizeInfo.truncationStart = n;
            }
        }
    }*/

    generateSpectrum();
    refreshDependentMembers(size);
}

void Spectrum_1D::truncate(size_t n)
{
    assert(n <= fidSizeInfo.initialSize);
    const size_t size = spectrum.size();
    fidSizeInfo.truncationStart = n;
    if (n > fidSizeInfo.zeroFilledTo) {
        fidSizeInfo.zeroFilledTo = Processing::nextPowerOf2(n);
    }
    generateSpectrum();
    refreshDependentMembers(size);
}

void Spectrum_1D::integrate(size_t start, size_t end) const
{
    double absoluteValue = integrateByTrapezoidRule(get_spectrum().subspan(start, end-start));
    if (integrals.empty()) {
        integrals.push_back(IntegralRecord{
                                        .leftEdge = 0,
                                        .rightEdge = 0,
                                        .absoluteValue = absoluteValue,
                                        .relativeValue = 1.0});
    }

    double relativeValue = (integrals[0].absoluteValue != 0.0)
                         ? absoluteValue / integrals[0].absoluteValue * integrals[0].relativeValue
                         : 0.0;

    integrals.push_back(IntegralRecord{
                                    .leftEdge = start,
                                    .rightEdge = end,
                                    .absoluteValue = absoluteValue,
                                    .relativeValue = relativeValue});
}

void Spectrum_1D::recalcIntegrals(size_t previousSpectrumSize) const
{
    if (integrals.empty()) return;
    const double mult = static_cast<double>(spectrum.size()) / previousSpectrumSize;

    integrals[0].absoluteValue *= mult; // increasing size by two by zero filling causes two fold increase
    // of y values in spectrum. To preserve integral values absolute value corresponding to 1.00 relative is
    // changed

    for (auto& i : std::span(integrals).subspan(1, integrals.size() - 1)) {
        i.leftEdge *= mult;
        i.rightEdge *= mult;
        i.absoluteValue = integrateByTrapezoidRule(get_spectrum().subspan(i.leftEdge, i.rightEdge - i.leftEdge));
        i.relativeValue = (integrals[0].absoluteValue != 0.0)
                        ? i.absoluteValue / integrals[0].absoluteValue * integrals[0].relativeValue
                        : 0.0;
    }
}

void Spectrum_1D::refreshDependentMembers(size_t previousSpectrumSize) const
{
    recalcIntegrals(previousSpectrumSize);
    autoFindPeaks();
}

void Spectrum_1D::autoFindPeaks() const
{
    using namespace PeakFinding;
    autoPeakList = simpleFindPeaks(get_spectrum(), calcTreshold(get_spectrum()));
}

// free functions
//----------------------------------------------------------------------------------------------------------------------------------

void recalcRelativeIntegralsValues(IntegralsVector& integrals, double valueOfOne)
{
    if (integrals.empty()) return;

    integrals[0].absoluteValue = valueOfOne;

    if (valueOfOne == 0.0) {
        for (auto& i : integrals) {
            i.relativeValue = 0.0;
    }
        return;
    }

    for (auto& i : integrals) {
        i.relativeValue = i.absoluteValue / valueOfOne;
    }
}

void resetIntegrals(IntegralsVector& integrals)
{
    integrals.clear();
}
