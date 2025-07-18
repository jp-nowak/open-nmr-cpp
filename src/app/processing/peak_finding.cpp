#include "peak_finding.h"

#include "utils.h"

#include <numeric>

using namespace PeakFinding;
using namespace Processing;


double PeakFinding::calcTreshold(ConstSpectrumSpan spectrum)
{
    ConstSpectrumSpan edge = spectrum.subspan(0, static_cast<size_t>(0.01 * spectrum.size()));
    size_t count = 0;
    double treshold = std::accumulate(edge.begin(), edge.end(), 0.0,
        [&count](SpectrumComplexValue lhs, SpectrumComplexValue rhs) -> double {
        if (rhs.real() > 0.0) {
            count++;
            return lhs.real() + rhs.real();
        } else {
            return lhs.real();
        }
    });

    return treshold / count * 10;
}


std::vector<Peak> PeakFinding::simpleFindPeaks(ConstSpectrumSpan spectrum, double treshold)
{
    std::vector<Peak> peaks{};
    Peak currentPeak{{0, 0}, {0, 0}, {0, 0}, {0, 0}};

    for (size_t x = 0; x < spectrum.size(); x++) {

        auto y = std::real(spectrum[x]);

        if (y < treshold) {

            if (currentPeak.rightTrough.x > 0) {

                if (y <= currentPeak.rightTrough.y) {

                    currentPeak.rightTrough.x = x;
                    currentPeak.rightTrough.y = y;

                } else {

                    peaks.push_back(currentPeak);
                    currentPeak = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};
                }

            } else if (currentPeak.leftTrough.x == 0) {

                currentPeak.leftTrough.x = x;
                currentPeak.leftTrough.y = y;

            } else if ((currentPeak.zenith.x > currentPeak.leftTrough.x)
                       and (currentPeak.leftTrough.x > 0)) {

                currentPeak.rightTrough.x = x;
                currentPeak.rightTrough.y = y;

            } else {

                currentPeak.leftTrough.x = x;
                currentPeak.leftTrough.y = y;

            }

        } else if (currentPeak.leftTrough.x > 0) {

            if ((currentPeak.zenith.x == 0)
                and (y >= currentPeak.zenith.y))
            {
                currentPeak.zenith.x = x;
                currentPeak.zenith.y = y;
            }

        } else if (currentPeak.rightTrough.x > 0) {

            peaks.push_back(currentPeak);
            currentPeak = {{0, 0}, {0, 0}, {0, 0}, {0, 0}};

        } else {

            currentPeak.leftTrough.x = x;
            currentPeak.leftTrough.y = y;

        }
    }

    for (auto& i : peaks) {

        i.zenith.x = i.leftTrough.x + indexOfMaxRealElemFromComplexArray(
            spectrum.subspan(i.leftTrough.x, i.rightTrough.x - i.leftTrough.x));
        i.zenith.y = std::real(spectrum[i.zenith.x]);

        if (i.zenith.x - i.leftTrough.x > i.rightTrough.x - i.zenith.x) {
            i.rightTrough.x = static_cast<size_t>(i.rightTrough.x + spectrum.size() * 0.0005);
            i.rightTrough.y = std::real(spectrum[i.rightTrough.x]);
        } else {
            i.leftTrough.x = static_cast<size_t>(i.leftTrough.x - spectrum.size() * 0.0005);
            i.leftTrough.y = std::real(spectrum[i.leftTrough.x]);
        }

        size_t leftMinIndex = 0;
        double leftMinValue = i.zenith.y;
        for (size_t j = i.zenith.x; j > i.leftTrough.x; j--) {
            if (std::real(spectrum[j]) <= leftMinValue) {
                leftMinValue = std::real(spectrum[j]);
                leftMinIndex = j;
            } else break;
        }

        size_t rightMinIndex = 0;
        double rightMinValue = i.zenith.y;
        for (size_t j = i.zenith.x; j < i.rightTrough.x; j++) {
            if (std::real(spectrum[j]) <= rightMinValue) {
                rightMinValue = std::real(spectrum[j]);
                rightMinIndex = j;
            } else break;
        }

        i.nidar.y = (rightMinValue < leftMinValue) ? rightMinValue : leftMinValue;
        i.nidar.x = (rightMinValue < leftMinValue) ? rightMinIndex : leftMinIndex;

        // i.nidar.x = i.leftTrough.x;
        // double max_angle = 0.0;
        // for (size_t j = i.leftTrough.x; j < i.rightTrough.x; j++) {
        //     if (j == i.zenith.x) continue;
        //     double angle = std::fabs(spectrum[i.zenith.x] - spectrum[j]) / std::fabs(i.zenith.x - j);
        //     if (angle < max_angle) {
        //         max_angle = angle;
        //         i.nidar.x = j;
        //     }

        // i.nidar.y = std::real(spectrum[i.nidar.x]);

        // }

    }

    return peaks;

}




