#ifndef SPECTRUMPAINTER_H
#define SPECTRUMPAINTER_H

#include "../spectrum/spectrum.h"


#include <QObject>
#include <QWidget>
#include <QPen>

#include <memory>
#include "../spectrum/spectrum_view.h"

class SpectrumDisplayer;

class SpectrumPainter : public QWidget
{
    Q_OBJECT
public:
    SpectrumPainter(std::vector<std::complex<double>>* spectrum, QWidget* parent = nullptr);

    friend SpectrumDisplayer;

    void paintEvent(QPaintEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;

    bool zoom(QPointF startPos, QPointF endPos);

    void changeSelectionWidth(QPointF x, QPointF origin);
    void setSelectionStart(QPointF x);
    void resetSelection();

    void resetZoom();

    SpectrumView spectrum;
private:

    void initialize();

    QPen spectrumPen;
    QRectF selectedRegion;
    double baselinePosition;
    double multiplier;
    double scalingFactor;
    bool displaySelection;

};

#endif // SPECTRUMPAINTER_H
