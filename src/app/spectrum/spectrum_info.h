#ifndef SPECTRUM_INFO_H
#define SPECTRUM_INFO_H

#include <string>

#ifdef DEBUG_COMP
#include <iostream>
#include <QDebug>
#endif

struct IntegralRecord
{
    size_t leftEdge; // left edge of integral range as data point number (0 - left edge)
    size_t rightEdge; // right edge of integral range as data point number
    double absoluteValue; // absolute value of integral
    double relativeValue; // relative value of integral = absolute / valueOfOne

    // static bool comparator(IntegralRecord a, IntegralRecord b)
    // {
    //     return ((a.leftEdge != b.leftEdge) ? (a.leftEdge < b.leftEdge) : (a.rightEdge < b.rightEdge));
    // }

    #ifdef DEBUG_COMP
    friend std::ostream& operator<<(std::ostream& os, IntegralRecord const& m)
    {
        return os << "IntegralRecord" << "\n"
               << "leftEdge: " << m.leftEdge << "\n"
               << "rightEdge: " << m.rightEdge << "\n"
               << "absoluteValue: " << m.absoluteValue << "\n"
               << "relativeValue: " << m.relativeValue << "\n";
    }

    friend QDebug operator<<(QDebug os, IntegralRecord const& m)
    {
        return os << "IntegralRecord" << "\n"
               << "leftEdge: " << m.leftEdge << "\n"
               << "rightEdge: " << m.rightEdge << "\n"
               << "absoluteValue: " << m.absoluteValue << "\n"
               << "relativeValue: " << m.relativeValue << "\n";
    }
    #endif

};

struct FidSizeInfo
{
    const size_t initialSize; // stores true size of fid ie. number of acquired data points
    size_t truncationStart; // number of not truncated data points
    size_t zeroFilledTo; // length of fid after zero filling
    double groupDelay;
};

struct SpectrumInfo
{
    double plot_right_Hz; // [Hz] value of x axis at right edge of spectrum
    double plot_left_Hz; // [Hz] value of x axis at left edge of spectrum
    double plot_right_ppm; // [ppm] value of x axis at right edge of spectrum
    double plot_left_ppm; // [ppm] value of x axis at left edge of spectrum
    double spectral_width; // [Hz] width of x axis
    double acquisition_time; // [s] time of acquisition, x axis of fid starts at 0 and ends at this value
    double obs_nucleus_freq; // [MHz] Larmor frequency of measured nucleus
    double dwell_time; // [s] time between two data points of fid
    double group_delay;
    double trimmed;
    std::string samplename;
    std::string nucleus; // measured nucleus of format XXNN X-nucleus symbol NN-mass
    std::string solvent; // solvent in which sample was solubilized

    #ifdef DEBUG_COMP
    friend std::ostream& operator<<(std::ostream& os, SpectrumInfo const& m)
    {
        return os << "SpectrumInfo" << "\n"
        << "plot_right_Hz: " << m.plot_right_Hz << "\n"
        << "plot_left_Hz: " << m.plot_left_Hz << "\n"
        << "plot_right_ppm: " << m.plot_right_ppm << "\n"
        << "plot_left_ppm: " << m.plot_left_ppm << "\n"
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
        return os << "SpectrumInfo" << "\n"
        << "plot_right_Hz: " << m.plot_right_Hz << "\n"
        << "plot_left_Hz: " << m.plot_left_Hz << "\n"
        << "plot_right_ppm: " << m.plot_right_ppm << "\n"
        << "plot_left_ppm: " << m.plot_left_ppm << "\n"
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
