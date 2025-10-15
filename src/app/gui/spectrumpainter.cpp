#include "spectrumpainter.h"
#include "../processing/peak_finding.h"
#include "../processing/utils.h"

#include <algorithm>
#include <utility>

#include <QPainter>
#include <QPolygon>
#include <QPolygonF>

#include <QPoint>
#include <QPointF>
#include <QPen>

#include <QDebug>
#include <QPaintEvent>
#include <QList>
#include <QTransform>

namespace
{
}

SpectrumPainter::SpectrumPainter(const Spectrum_1D* spectrum_, QWidget* parent)
    : QWidget{parent}
    , pointerToSpectrum{spectrum_}
    , selectedRegion{0, 0, 0, static_cast<qreal>(height())}
    , baselinePosition{0.125}
    , multiplier{1}
    , scalingFactor{1}
    , displaySelection{false}
    , selectionColor{}
    , maximum{Processing::findRealMaximum(spectrum_->get_spectrum()) * 1.05}
    , startPoint_{0}
    , endPoint_{spectrum_->get_spectrum().size()}
    , currentSpectrumSize{spectrum_->get_spectrum().size()}

{
    spectrumPen.setCosmetic(true);
    spectrumPen.setWidth(1);
    spectrumPen.setColor(QColor(255, 0, 0));
    spectrumPen.setCapStyle(Qt::RoundCap);
}

void SpectrumPainter::changeSelectionWidth(QPointF x, QPointF origin)
{
    const double newWidth = mapFromGlobal(origin).x() - mapFromGlobal(x).x();

    if (newWidth < 0) { // mouse moves to the right
        selectedRegion.setWidth(-newWidth);
    }  else { // mouse moves to the left
        selectedRegion.setX(mapFromGlobal(x).x());
        selectedRegion.setWidth(newWidth);
    }
    displaySelection = true;
    update();
}

void SpectrumPainter::setSelectionStart(QPointF x, const QColor& selectionColor)
{
    selectedRegion.setX(mapFromGlobal(x).x());
    displaySelection = false;
    this->selectionColor = selectionColor;
}

void SpectrumPainter::resetSelection()
{
    selectedRegion = QRect{};
    displaySelection = false;
    update();
}

bool SpectrumPainter::zoom(QPointF startPos, QPointF endPos)
{
    double start = mapFromGlobal(startPos).x();
    double end = mapFromGlobal(endPos).x();

    const size_t oldStart = startPoint_;
    const size_t oldEnd = endPoint_;

    if (start > end) {
        std::swap(start, end);
    }

    start = (start < 0) ? 0 : start;
    end = (end > width()) ? width() : end;

    const size_t size = endPoint_ - startPoint_;

    endPoint_ = startPoint_ + std::floor(end / width() * (size - 1));
    startPoint_ += std::ceil(start / width() * (size - 1));

    if (startPoint_ + 5 > endPoint_) {
        startPoint_ = oldStart;
        endPoint_ = oldEnd;
        return false;
    } // stops user from zooming to close


    update();

    return true;
}

void SpectrumPainter::resetZoom()
{
    startPoint_ = 0;
    endPoint_ = pointerToSpectrum->get_spectrum().size();
    update();
}

void SpectrumPainter::recalculateDisplayRange()
{
    assert(currentSpectrumSize != 0);
    if (size_t n = pointerToSpectrum->get_spectrum().size(); n == currentSpectrumSize) {
        return;
    } else {
        startPoint_ = static_cast<double>(startPoint_) / currentSpectrumSize * n;
        endPoint_ = static_cast<double>(endPoint_) / currentSpectrumSize * n;
        currentSpectrumSize = n;
    }
}

std::pair<size_t, size_t> SpectrumPainter::selectionRangeToDataPointsOfSpectrum(QPointF startPos, QPointF endPos) const
{
    double start = mapFromGlobal(startPos).x();
    double end = mapFromGlobal(endPos).x();

    if (start > end) {
        std::swap(start, end);
    }

    start = (start < 0) ? 0 : start;
    end = (end > width()) ? width() : end;

    const size_t size = endPoint_ - startPoint_;

    size_t right = startPoint_ + std::floor(end / width() * (size - 1));
    size_t left = startPoint_ + std::ceil(start / width() * (size - 1));

    return{left, right};
}

void SpectrumPainter::paintEvent(QPaintEvent* e)
{
    // TODO maybe when number of points is huge it should draw every second/third one
    recalculateDisplayRange();
    auto spectrum = pointerToSpectrum->get_spectrum().subspan(startPoint_, endPoint_ - startPoint_);
    QPainter painter(this);
    painter.drawPolygon(e->rect().adjusted(1,1,-1,-1));

    const double width_ = e->rect().width();
    const double height_ = e->rect().height();

    auto xPos = [&](double x) -> double {return x / spectrum.size() * width_;};
    auto yPos = [&](double y) -> double {return (1 - (y + baselinePosition * (1 + baselinePosition) * maximum) / ((1 + baselinePosition) * maximum)) * height_;};

    // drawing baseline
    painter.drawLine(QPointF(0, (1 - baselinePosition) * height_), QPointF(width_, (1 - baselinePosition) * height_));

    // drawing selection region
    if (displaySelection) {
        selectedRegion.setHeight(height());
        painter.fillRect(selectedRegion, selectionColor);
    }

    // drawing spectrum
    QPolygonF spectrumPolygon;
    for (size_t i = 0; i < spectrum.size(); i++) {
        spectrumPolygon << QPointF(xPos(i), yPos(spectrum[i].real() * scalingFactor));
    }
    painter.setPen(spectrumPen);
    painter.drawPolyline(spectrumPolygon);

#ifdef PEAK_FINDING_TEST
    // drawing found peaks
    for (auto& i : pointerToSpectrum->autoPeakList) {
        auto peakTickXVal = (i.zenith.x - startPoint_);
        painter.drawLine(QPointF(xPos(peakTickXVal), yPos(maximum * 0.8)), QPointF(xPos(peakTickXVal), yPos(maximum)));
    }
    painter.setPen(QColor("green"));
    for (auto& i : pointerToSpectrum->autoPeakList) {
        auto peakTickXVal = (i.leftTrough.x - startPoint_);
        painter.drawLine(QPointF(xPos(peakTickXVal), yPos(maximum * 0.8)), QPointF(xPos(peakTickXVal), yPos(maximum)));
    }
    for (auto& i : pointerToSpectrum->autoPeakList) {
        auto peakTickXVal = (i.rightTrough.x - startPoint_);
        painter.drawLine(QPointF(xPos(peakTickXVal), yPos(maximum * 0.8)), QPointF(xPos(peakTickXVal), yPos(maximum)));
    }
    painter.setPen(QColor("black"));
    for (auto& i : pointerToSpectrum->autoPeakList) {
        auto peakTickXVal = (i.nidar.x - startPoint_);
        painter.drawLine(QPointF(xPos(peakTickXVal), yPos(maximum * 0.8)), QPointF(xPos(peakTickXVal), yPos(maximum)));
    }
#endif

}

void SpectrumPainter::wheelEvent(QWheelEvent* e)
{
    const double increment = 1.1;
    if (e->angleDelta().y() > 0) { // mouse wheel is turned from user, spectrum values are scaled up
        if (scalingFactor > 1000000) return;
        scalingFactor *= increment;
        emit wheelTurned(increment);
    } else { // mouse wheel is turned towards user, spectrum values are scaled down
        // if (scalingFactor < 0.01) return;
        scalingFactor /= increment;
        emit wheelTurned(-increment);
    }
    update();
}

