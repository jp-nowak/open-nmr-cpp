#ifndef SPECTRUMPAINTER_H
#define SPECTRUMPAINTER_H

#include "../spectrum/spectrum.h"


#include <QObject>
#include <QWidget>
#include <QPen>
#include <QColor>

class SpectrumDisplayer_1D;

class SpectrumPainter : public QWidget
{
    Q_OBJECT
public:
    SpectrumPainter(const Spectrum_1D* spectrum_, QWidget* parent = nullptr);

    friend SpectrumDisplayer_1D;

    void paintEvent(QPaintEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;

    bool zoom(QPointF startPos, QPointF endPos);
    void setZoom(size_t start, size_t end);

    void changeSelectionWidth(QPointF x, QPointF origin);
    void setSelectionStart(QPointF x, const QColor& selectionColor = QColor(255, 0, 0, 100));
    void resetSelection();

    void resetZoom();
    void recalculateDisplayRange();

    //! returns data point numbers in spectrum which correspond to current selection
    std::pair<size_t, size_t> selectionRangeToDataPointsOfSpectrum(QPointF startPos, QPointF endPos) const;

    //! returns range of points currently displayed [startPoint, endPoint]
    std::pair<size_t, size_t> currentSelection() const;

signals:
    void wheelTurned(double);


private:

    void initialize();

    const Spectrum_1D* pointerToSpectrum;

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
