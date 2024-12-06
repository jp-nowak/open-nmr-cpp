#ifndef SPECTRUMPAINTER_H
#define SPECTRUMPAINTER_H

#include "../spectrum/spectrum.h"


#include <QObject>
#include <QWidget>
#include <QPen>

#include <memory>


class SpectrumPainter : public QWidget
{
    Q_OBJECT
public:
    SpectrumPainter(std::unique_ptr<Spectrum>&& new_experiment, QWidget* parent = nullptr);

    void paintEvent(QPaintEvent* e);

    std::unique_ptr<Spectrum> experiment;
private:



    QPen spectrumPen;

    double baselinePosition;
    double multiplier;
    double scalingFactor;

    size_t maxSpectrumElemIndex;
    size_t displayBegin;
    size_t displayEnd;
};

#endif // SPECTRUMPAINTER_H
