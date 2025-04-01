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
    SpectrumPainter(const Spectrum* spectrum_, QWidget* parent = nullptr);

    friend SpectrumDisplayer;

    void paintEvent(QPaintEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;

    bool zoom(QPointF startPos, QPointF endPos);

    void changeSelectionWidth(QPointF x, QPointF origin);
    void setSelectionStart(QPointF x);
    void resetSelection();

    void resetZoom();
    void recalculateDisplayRange();

private:

    void initialize();

    const Spectrum* pointerToSpectrum;

    QPen spectrumPen;
    QRectF selectedRegion;
    double baselinePosition;
    double multiplier;
    double scalingFactor;
    bool displaySelection;
    const double maximum;

    size_t startPoint_;
    size_t endPoint_;
    size_t currentSpectrumSize;
};

#endif // SPECTRUMPAINTER_H
