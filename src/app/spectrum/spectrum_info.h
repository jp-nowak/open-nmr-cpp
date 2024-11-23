#ifndef SPECTRUM_INFO_H
#define SPECTRUM_INFO_H

#include <string>

#ifdef DEBUG__
#include <iostream>
#include <QDebug>
#endif

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

    #ifdef DEBUG__
    friend std::ostream& operator<<(std::ostream& os, SpectrumInfo const& m)
    {
        return os
        << "plot_begin_Hz: " << m.plot_begin_Hz << "\n"
        << "plot_end_Hz: " << m.plot_end_Hz << "\n"
        << "plot_begin_ppm: " << m.plot_begin_ppm << "\n"
        << "plot_end_ppm: " << m.plot_end_ppm << "\n"
        << "spectral_width: " << m.spectral_width << "\n"
        << "acquisition_time: " << m.acquisition_time << "\n"
        << "obs_nucleus_freq: " << m.obs_nucleus_freq << "\n"
        << "dwell_time: " << m.dwell_time << "\n"
        << "group_delay: " << m.group_delay << "\n"
        << "trimmed: " << m.trimmed << "\n"
        << "samplename: " << m.samplename << "\n"
        << "nucleus: " << m.nucleus << "\n"
        << "solvent: " << m.solvent << "\n";
    }

    friend QDebug operator<<(QDebug os, SpectrumInfo const& m)
    {
        return os
        << "plot_begin_Hz: " << m.plot_begin_Hz << "\n"
        << "plot_end_Hz: " << m.plot_end_Hz << "\n"
        << "plot_begin_ppm: " << m.plot_begin_ppm << "\n"
        << "plot_end_ppm: " << m.plot_end_ppm << "\n"
        << "spectral_width: " << m.spectral_width << "\n"
        << "acquisition_time: " << m.acquisition_time << "\n"
        << "obs_nucleus_freq: " << m.obs_nucleus_freq << "\n"
        << "dwell_time: " << m.dwell_time << "\n"
        << "group_delay: " << m.group_delay << "\n"
        << "trimmed: " << m.trimmed << "\n"
        << "samplename: " << m.samplename << "\n"
        << "nucleus: " << m.nucleus << "\n"
        << "solvent: " << m.solvent << "\n";
    }
    #endif

};




#endif // SPECTRUM_INFO_H
