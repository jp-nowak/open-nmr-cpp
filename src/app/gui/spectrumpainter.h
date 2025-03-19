#ifndef SPECTRUMPAINTER_H
#define SPECTRUMPAINTER_H

#include "../spectrum/spectrum.h"


#include <QObject>
#include <QWidget>
#include <QPen>

#include <span>
#include <memory>

class SpectrumDisplayer;

class SpectrumPainter : public QWidget
{
    Q_OBJECT
public:
    SpectrumPainter(std::span<SpectrumComplexValue const> spectrum, QWidget* parent = nullptr);

    friend SpectrumDisplayer;

    void paintEvent(QPaintEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;

    bool zoom(QPointF startPos, QPointF endPos);

    void changeSelectionWidth(QPointF x, QPointF origin);
    void setSelectionStart(QPointF x);
    void resetSelection();

    void resetZoom();

    std::span<std::complex<double> const> spectrum;
    const std::span<std::complex<double> const> spectrumFullSpan;

private:

    void initialize();

    QPen spectrumPen;
    QRectF selectedRegion;
    double baselinePosition;
    double multiplier;
    double scalingFactor;
    bool displaySelection;
    const double maximum;
};

#endif // SPECTRUMPAINTER_H
