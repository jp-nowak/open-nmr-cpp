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
    double newWidth = mapFromGlobal(origin).x() - mapFromGlobal(x).x();

    if (newWidth < 0) { // mouse moves to the right
        qDebug() << "1" << newWidth;
        selectedRegion.setWidth(-newWidth);
    }  else { // mouse moves to the left
        qDebug() << "2" << newWidth;
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
}

void SpectrumPainter::zoom(QPointF startPos, QPointF endPos)
{
    double start = mapFromGlobal(startPos).x();
    double end = mapFromGlobal(endPos).x();


    if (start > end) {
        std::swap(start, end);
    }

    if ((start < 0) or (end > width())) {
        return;
    }

    size_t startPoint = std::ceil(start / width() * (spectrum.size() - 1));
    size_t endPoint = std::floor(end / width() * (spectrum.size() - 1));

    if (startPoint + 5 > endPoint) {return;}

    qDebug() << "zoom" << startPoint << endPoint;
    spectrum.setRange(startPoint, endPoint);
    update();

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

        // highest value that will be displayed on y axis, scrolling with mouse wheel will depend on changing it
        double maximum = spectrum[spectrum.maxElemIndex].real() * 1.05;

        // setting coordinate system in such way that points from spectrum can be displayed without transformations
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

