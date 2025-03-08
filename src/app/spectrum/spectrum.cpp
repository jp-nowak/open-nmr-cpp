#include "spectrum.h"
#include "../processing/general.h"

Spectrum::Spectrum(const SpectrumInfo& info, const std::vector<std::complex<double>>& fid, const std::vector<std::complex<double>>& spectrum)
: info{info}
, spectrum{spectrum}
, fid{fid}
, phaseCorrection{Processing::Ph0{.ph0 = 0}, Processing::Ph1{.ph1 = 0, .pivot = 50}}
{}

Spectrum Spectrum::from_file_read_result(FileIO::FileReadResult result)
{
    return Spectrum(result.info, result.fids[0], Processing::generate_spectrum_from_fid(result.fids[0]));
}

std::unique_ptr<Spectrum> Spectrum::pointer_from_file_read_result(FileIO::FileReadResult result)
{
    return std::make_unique<Spectrum>(result.info, result.fids[0], Processing::generate_spectrum_from_fid(result.fids[0]));
}

const std::vector<std::complex<double>>& Spectrum::get_spectrum()
{
    return spectrum;
}

const Processing::Phase& Spectrum::getPhase()
{
    return phaseCorrection;
}

void Spectrum::setPh0(const Processing::Ph0& phase)
{
    using namespace Processing;

    spectrum *= Ph0(phase.ph0 - phaseCorrection.ph0.ph0);
    phaseCorrection.ph0.ph0 = phase.ph0;
}

void Spectrum::setPh1(const Processing::Ph1& phase)
{
    using namespace Processing;

    if (phase.pivot == phaseCorrection.ph1.pivot) {
        spectrum *= Ph1(phase.ph1 - phaseCorrection.ph1.ph1, phase.pivot);
        phaseCorrection.ph1.ph1 = phase.ph1;
    } else {
        spectrum *= Ph1(-phaseCorrection.ph1.ph1, phaseCorrection.ph1.pivot);
        spectrum *= Ph1(phase.ph1, phase.pivot);
        phaseCorrection.ph1 = phase;
    }
}
