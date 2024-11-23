#ifndef SPECTRUM_INFO_H
#define SPECTRUM_INFO_H

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
    std::string nucleus; // measured nucleus of format XXNN X-nucleus symbol NN-mass
    std::string solvent; // solvent in which sample was solubilized

};

#endif // SPECTRUM_INFO_H
