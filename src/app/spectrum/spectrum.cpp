#include "spectrum.h"
#include "../processing/general.h"
#include "../processing/zero_filling.h"

#include <QDebug>

#include <algorithm>
#include <cassert>

using namespace Processing;

Spectrum::Spectrum(const SpectrumInfo& info, const std::vector<std::complex<double>>& fid)
: info{info}
, fid{fid}
, phaseCorrection{Ph0{.ph0 = 0},
                  Ph1{.ph1 = 0, .pivot = 50}}
, fidSizeInfo{.initialSize = fid.size(),
              .truncationStart = fid.size(),
              .zeroFilledTo = nextPowerOf2(fid.size()),
              .groupDelay = info.group_delay}

{
    generateSpectrum();
    if (!(info.group_delay == 0.0)) {
        double decimalDelay = info.group_delay - static_cast<size_t>(info.group_delay);
        setPh1(Ph1{.ph1 = -decimalDelay, .pivot = 75});
    }
}

void Spectrum::generateSpectrum()
{
    spectrum = generate_spectrum_from_fid(std::span<FidComplexValue const>(this->fid), fidSizeInfo);
    restorePhase();
}

void Spectrum::restorePhase()
{
    spectrum *= phaseCorrection.ph0;
    spectrum *= phaseCorrection.ph1;
}

std::unique_ptr<Spectrum> Spectrum::pointer_from_file_read_result(FileIO::FileReadResult result)
{
    return std::make_unique<Spectrum>(result.info, result.fids[0]);
}

std::span<SpectrumComplexValue const> Spectrum::get_spectrum() const
{
    return {spectrum};
}

const Phase& Spectrum::getPhase() const
{
    return phaseCorrection;
}

const FidSizeInfo& Spectrum::getFidSizeInfo() const
{
    return fidSizeInfo;
}

void Spectrum::setPh0(const Ph0& phase)
{
    spectrum *= Ph0(phase.ph0 - phaseCorrection.ph0.ph0);
    phaseCorrection.ph0.ph0 = phase.ph0;
}

void Spectrum::setPh1(const Ph1& phase)
{
    if (phase.pivot == phaseCorrection.ph1.pivot) {
        spectrum *= Ph1(phase.ph1 - phaseCorrection.ph1.ph1, phase.pivot);
        phaseCorrection.ph1.ph1 = phase.ph1;
    } else {
        spectrum *= Ph1(-phaseCorrection.ph1.ph1, phaseCorrection.ph1.pivot);
        spectrum *= Ph1(phase.ph1, phase.pivot);
        phaseCorrection.ph1 = phase;
    }
}

void Spectrum::zeroFill(size_t n)
{
    assert(std::find(POWERS_OF_TWO.begin(), POWERS_OF_TWO.end(), n) != POWERS_OF_TWO.end());
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
}

void Spectrum::truncate(size_t n)
{
    assert(n <= fidSizeInfo.initialSize);
    fidSizeInfo.truncationStart = n;
    generateSpectrum();
}
