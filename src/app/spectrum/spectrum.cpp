#include "spectrum.h"
#include "../processing/general.h"

Spectrum::Spectrum(const SpectrumInfo& info, const std::vector<std::complex<double>>& fid, const std::vector<std::complex<double>>& spectrum)
  : info{info},
    fid{fid},
    spectrum{spectrum}
{}

Spectrum Spectrum::from_file_read_result(FileIO::FileReadResult result)
{
    return Spectrum(result.info, result.fids[0], Processing::generate_spectrum_from_fid(result.fids[0]));
}
