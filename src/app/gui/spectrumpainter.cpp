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

SpectrumPainter::SpectrumPainter(std::vector<std::complex<double>>* spectrum, QWidget* parent)
    : QWidget{parent}
    , spectrum{spectrum}
    , selectedRegion{0, 0, 0, static_cast<qreal>(height())}
    , baselinePosition{0.125}
    , multiplier{1}
    , scalingFactor{1}
    , displaySelection{false}

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

    const size_t startPoint = std::ceil(start / width() * (spectrum.size() - 1));
    const size_t endPoint = std::floor(end / width() * (spectrum.size() - 1));

    if (startPoint + 5 > endPoint) {return false;} // stops user from zooming to close

    spectrum.setRange(startPoint, endPoint);
    update();
    return true;

}

void SpectrumPainter::resetZoom()
{
    spectrum.reset();
    update();
}

void SpectrumPainter::paintEvent(QPaintEvent* e)
{
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

        // highest value that will be displayed on y axis
        // const double maximum = spectrum[spectrum.maxElemIndex].real() * 1.05;
        const double maximum = spectrum.trueRealMaximum * 1.05;
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

