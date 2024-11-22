#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <vector>
#include <complex>
#include <string>

struct SpectrumInfo
{
    double plot_begin_Hz; // [Hz] value of x axis at right edge of spectrum
    double plot_end_Hz; // [Hz] value of x axis at left edge of spectrum
    double plot_begin_ppm; // [ppm] value of x axis at right edge of spectrum
    double plot_end_ppm; // [ppm] value of x axis at left edge of spectrum
    double spectral_width; // [Hz] width of x axis
    double acquisition_time; // [s] time of acquisition, x axis of fid starts at 0 and ends at this value
    double obs_nucleus_freq; // [MHz] Larmor frequency of measured nucleus
    double dwell_time; // [s] time between two data points of fid
    double group_delay;
    double trimmed;
    std::string samplename;

};

class Spectrum
{
public:
    Spectrum(const SpectrumInfo& info, const std::vector<std::complex<double>>& fid, const std::vector<std::complex<double>>& spectrum);

    const SpectrumInfo info;

private:
    std::vector<std::complex<double>> fid;
    std::vector<std::complex<double>> spectrum;
};

#endif // SPECTRUM_H
