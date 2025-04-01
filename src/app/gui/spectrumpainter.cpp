#include "spectrumpainter.h"

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

auto findRealMaximumFromVectorOfComplex = [](auto i){
    return i[std::max_element(std::begin(i), std::end(i), [](auto& a, auto& b){
        return a.real() < b.real();
    }) - begin(i)].real();
};

}

SpectrumPainter::SpectrumPainter(const Spectrum* spectrum_, QWidget* parent)
    : QWidget{parent}
    , pointerToSpectrum{spectrum_}
    , selectedRegion{0, 0, 0, static_cast<qreal>(height())}
    , baselinePosition{0.125}
    , multiplier{1}
    , scalingFactor{1}
    , displaySelection{false}
    , maximum{findRealMaximumFromVectorOfComplex(spectrum_->get_spectrum()) * 1.05}
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
    update();
}

void SpectrumPainter::setSelectionStart(QPointF x)
{
    selectedRegion.setX(mapFromGlobal(x).x());
    displaySelection = true;
}

void SpectrumPainter::resetSelection()
{
    selectedRegion.setWidth(0.0);
    displaySelection = false;
    update();
}

bool SpectrumPainter::zoom(QPointF startPos, QPointF endPos)
{
    double start = mapFromGlobal(startPos).x();
    double end = mapFromGlobal(endPos).x();

    if (start > end) {
        std::swap(start, end);
    }

    start = (start < 0) ? 0 : start;
    end = (end > width()) ? width() : end;

    const size_t size = endPoint_ - startPoint_;

    endPoint_ = startPoint_ + std::floor(end / width() * (size - 1));
    startPoint_ += std::ceil(start / width() * (size - 1));

    if (startPoint_ + 5 > endPoint_) {return false;} // stops user from zooming to close


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

void SpectrumPainter::paintEvent(QPaintEvent* e)
{
    recalculateDisplayRange();
    auto spectrum = pointerToSpectrum->get_spectrum().subspan(startPoint_, endPoint_ - startPoint_);
    QPainter painter(this);
    painter.drawPolygon(e->rect().adjusted(1,1,-1,-1));
    QPolygonF baseLine;
    baseLine << QPointF(0, e->rect().height() * (1 - baselinePosition))
             << QPointF(e->rect().width(), e->rect().height() * (1 - baselinePosition));
    painter.drawPolyline(baseLine);

    // drawing selection region
    if (displaySelection) {
        selectedRegion.setHeight(height());
        painter.fillRect(selectedRegion, QColor(255, 0, 0, 100));
    }

    // setting coordinate system in such way that points from spectrum can be displayed without many transformations
    painter.setWindow(0, -maximum * multiplier, (spectrum.size() - 1) * multiplier, (maximum * (1/(1-baselinePosition))) * multiplier);
    painter.scale(1, -1);

    // drawing spectrum
    QPolygonF spectrumPolygon{};
    for (size_t i = 0; i < spectrum.size(); i++) {
        spectrumPolygon << QPointF(i * multiplier, spectrum[i].real() * multiplier * scalingFactor);
    }
    painter.setPen(spectrumPen);
    painter.drawPolyline(spectrumPolygon);
}

void SpectrumPainter::wheelEvent(QWheelEvent* e)
{
    if (e->angleDelta().y() > 0) { // mouse wheel is turned from user, spectrum values are scaled up
        scalingFactor *= 1.1;
    } else { // mouse wheel is turned towards user, spectrum values are scaled down
        scalingFactor /= 1.1;
    }
    update();
}

