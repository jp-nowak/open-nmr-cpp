#include "spectrum.h"
#include "../processing/general.h"
#include "../processing/zero_filling.h"

#include <algorithm>
#include <cassert>

using namespace Processing;

Spectrum::Spectrum(const SpectrumInfo& info, const std::vector<std::complex<double>>& fid)
: info{info}
, fid{fid}
, phaseCorrection{Ph0{.ph0 = 0}, Ph1{.ph1 = 0, .pivot = 50}}
{
    zeroFillToNextPowerOf2(this->fid);

    if (!(info.group_delay == 0.0)) {
        std::rotate(this->fid.begin(), this->fid.begin() + static_cast<size_t>(info.group_delay), this->fid.end());
    }
    generateSpectrum();
}

void Spectrum::generateSpectrum()
{
    spectrum = generate_spectrum_from_fid(this->fid);

    if (!(info.group_delay == 0.0)) {
        double decimalDelay = info.group_delay - static_cast<size_t>(info.group_delay);
        setPh1(Ph1{.ph1 = -decimalDelay, .pivot = 75});
    }
}

std::unique_ptr<Spectrum> Spectrum::pointer_from_file_read_result(FileIO::FileReadResult result)
{
    return std::make_unique<Spectrum>(result.info, result.fids[0]);
}

const std::vector<std::complex<double>>& Spectrum::get_spectrum() const
{
    return spectrum;
}

const Phase& Spectrum::getPhase() const
{
    return phaseCorrection;
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

    // put delayed part again in the beginning
    if (!(info.group_delay == 0.0)) {
        std::rotate(fid.rbegin(), fid.rbegin() + static_cast<size_t>(info.group_delay), fid.rend()); // rotate right
    }

    zeroFillToNumber(fid, n);

    // put delayed part at the end
    if (!(info.group_delay == 0.0)) {
        std::rotate(fid.begin(), fid.begin() + static_cast<size_t>(info.group_delay), fid.end()); // rotate left
    }


}
