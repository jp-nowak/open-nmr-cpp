#include "spectrum.h"

Spectrum::Spectrum(const SpectrumInfo& info, const std::vector<std::complex<double>>& fid, const std::vector<std::complex<double>>& spectrum)
: info{info},
fid{fid},
spectrum{spectrum}
{}

