#ifndef SPECTRUMPAINTER_H
#define SPECTRUMPAINTER_H

#include "../spectrum/spectrum.h"


#include <QObject>
#include <QWidget>
#include <QPen>

#include <memory>
#include "../spectrum/spectrum_view.h"


class SpectrumPainter : public QWidget
{
    Q_OBJECT
public:
    SpectrumPainter(std::vector<std::complex<double>>* spectrum, QWidget* parent = nullptr);

    void paintEvent(QPaintEvent* e);

    void zoom(QPointF startPos, QPointF endPos);
    void resetZoom();

    SpectrumView spectrum;
private:

    void initialize();

    QPen spectrumPen;

    double baselinePosition;
    double multiplier;
    double scalingFactor;


};

#endif // SPECTRUMPAINTER_H
