#ifndef SPECTRUMPAINTER_H
#define SPECTRUMPAINTER_H

#include "../spectrum/spectrum.h"


#include <QObject>
#include <QWidget>
#include <QPen>
#include <QColor>

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
    void setSelectionStart(QPointF x, const QColor& selectionColor = QColor(255, 0, 0, 100));
    void resetSelection();

    void resetZoom();
    void recalculateDisplayRange();

    // returns data point numbers in spectrum which correspond to current selection
    std::pair<size_t, size_t> selectionRangeToDataPointsOfSpectrum(QPointF startPos, QPointF endPos) const;

signals:
    void wheelTurned(double);


private:

    void initialize();

    const Spectrum* pointerToSpectrum;

    QPen spectrumPen;
    QRectF selectedRegion;
    double baselinePosition;
    double multiplier;
    double scalingFactor;
    bool displaySelection;
    QColor selectionColor;
    const double maximum;

    size_t startPoint_;
    size_t endPoint_;
    size_t currentSpectrumSize;
};

#endif // SPECTRUMPAINTER_H
